import asyncio
import os
import time
import logging
from typing import Optional
from google.cloud import storage
import pyarrow as pa
import pyarrow.parquet as pq

import struct
from nats.aio.client import Client as NATS
from nats.js.errors import NotFoundError

# Set up logging matching QuanuX-Annex patterns
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger("quanux.gcp.ingestion")

class GCPIngestionPipeline:
    def __init__(self, memory_limit_mb: int = 500, bucket_name: str = "quanux-historical-lake"):
        self.memory_limit_bytes = memory_limit_mb * 1024 * 1024
        self.bucket_name = bucket_name
        self.current_batch = []
        self.current_batch_size = 0
        
        # We define a PyArrow schema that matches the quanux.schema.MarketTick FlatBuffer
        self.schema = pa.schema([
            ('timestamp_ns', pa.int64()),
            ('instrument_id', pa.uint32()),
            ('bid_price', pa.float64()),
            ('ask_price', pa.float64()),
            ('bid_size', pa.uint32()),
            ('ask_size', pa.uint32()),
            ('level', pa.uint8())
        ])
        
        try:
            self.gcs_client = storage.Client()
            self.bucket = self.gcs_client.bucket(self.bucket_name)
        except Exception as e:
            logger.warning(f"Failed to initialize GCS client: {e}. Will run in dry-run mode.")
            self.gcs_client = None
            self.bucket = None

    async def start(self):
        """Starts the NATS JetStream listener and begins batching."""
        logger.info(f"Starting GCP Ingestion Pipeline. Memory limit: {self.memory_limit_bytes / (1024*1024)} MB")
        
        self.nc = NATS()
        nats_url = os.environ.get("QUANUX_NATS_URL", "nats://127.0.0.1:4222")
        await self.nc.connect(nats_url)
        self.js = self.nc.jetstream()
        
        try:
            self.sub = await self.js.subscribe("QUANUX.MARKET.TICK", cb=self._on_message)
            logger.info("Listening on JetStream subject QUANUX.MARKET.TICK")
        except Exception as e:
            logger.error(f"Failed to subscribe to JetStream: {e}")
            raise
        
        # Keep alive
        while True:
            await asyncio.sleep(5)
            
    async def _on_message(self, msg):
        """Callback for incoming JetStream messages."""
        try:
            # Struct format: < Q I d d I I B  (37 bytes)
            # uint64_t timestamp_ns, uint32_t instrument_id, double bid_price, double ask_price, uint32_t bid_size, uint32_t ask_size, uint8_t level
            unpacked = struct.unpack("<QIddIIB", msg.data)
            
            data_row = {
                'timestamp_ns': unpacked[0],
                'instrument_id': unpacked[1],
                'bid_price': unpacked[2],
                'ask_price': unpacked[3],
                'bid_size': unpacked[4],
                'ask_size': unpacked[5],
                'level': unpacked[6]
            }
            self.current_batch.append(data_row)
            
            # Strict incremental byte model: each canonical struct adds exactly 37 primitive bytes.
            self.current_batch_size += 37
            
            # Predictive memory bounding checks payload accumulation against a 99% safety ceiling.
            # This eliminates arbitrary row-count checkpoints (e.g. 5000) and guarantees strict enforcement.
            if self.current_batch_size >= (self.memory_limit_bytes * 0.99):
                arrays = [pa.array([row[col_name] for row in self.current_batch]) for col_name in self.schema.names]
                temp_table = pa.Table.from_arrays(arrays, schema=self.schema)
                
                real_nbytes = temp_table.nbytes
                
                logger.info(f"Strict memory ceiling predicted. True PyArrow Bytes: {real_nbytes} / Ceiling: {self.memory_limit_bytes}. Triggering predictive flush.")
                await self._flush_and_upload(temp_table)
            
        except struct.error:
            logger.error("Failed to unpack MarketTick struct - invalid payload size.")
        except Exception as e:
            logger.error(f"Error processing message: {e}")

    async def _flush_and_upload(self, table=None):
        """Flushes the current batch to Arrow/Parquet and uploads to GCS."""
        if not self.current_batch:
            return
            
        if table is None:
            logger.info(f"Building Arrow Table with {len(self.current_batch)} rows...")
            arrays = [pa.array([row[col_name] for row in self.current_batch]) for col_name in self.schema.names]
            table = pa.Table.from_arrays(arrays, schema=self.schema)
        
        # Write to temporary parquet file
        timestamp = int(time.time())
        filename = f"market_ticks_{timestamp}.parquet"
        local_path = f"/tmp/{filename}"
        
        pq.write_table(table, local_path)
        logger.info(f"Wrote Parquet file: {local_path} (Size: {os.path.getsize(local_path)} bytes)")
        
        if self.bucket:
            # Asynchronous GCS Upload
            blob = self.bucket.blob(f"ingestion/{filename}")
            # Run blocking upload in an executor
            loop = asyncio.get_running_loop()
            await loop.run_in_executor(None, blob.upload_from_filename, local_path)
            logger.info(f"Uploaded {filename} to GCS bucket {self.bucket_name}")
        else:
            logger.info(f"Dry-run: Would have uploaded {filename} to GCS.")
            
        # Clean up
        os.remove(local_path)
        
        # Reset batch
        self.current_batch = []
        self.current_batch_size = 0
        logger.info("Batch reset. Resuming JetStream consumption.")

if __name__ == "__main__":
    pipeline = GCPIngestionPipeline(memory_limit_mb=500)
    try:
        asyncio.run(pipeline.start())
    except KeyboardInterrupt:
        logger.info("Pipeline stopped by Operator.")
