import asyncio
import os
import time
import logging
from typing import Optional
from google.cloud import storage
import pyarrow as pa
import pyarrow.parquet as pq

# QuanuX Internal Imports (Simulated/Mocks for now as we establish the skeleton)
# We will use the standard pattern for JetStream ingestion.
# from quanux.annex import nats_client
# from quanux.schema import MarketTick

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
            ('symbol', pa.string()),
            ('bid', pa.float64()),
            ('ask', pa.float64()),
            ('bid_size', pa.int32()),
            ('ask_size', pa.int32()),
            ('venue_id', pa.int8())
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
        
        # Simulated NATS Subscription setup
        # nc = await nats_client.connect()
        # js = nc.jetstream()
        # sub = await js.subscribe("PQS.TICK.>", cb=self._on_message)
        
        logger.info("Listening on JetStream subject PQS.TICK.>")
        
        # Keep alive
        while True:
            await asyncio.sleep(5)
            # Periodic flush check could go here if elapsed time exceeds a threshold
            
    async def _on_message(self, msg):
        """Callback for incoming JetStream messages."""
        # raw_data = msg.data
        # tick = MarketTick.GetRootAsMarketTick(raw_data, 0)
        
        # Simulated extraction
        data_row = {
            'timestamp_ns': time.time_ns(),
            'symbol': 'ESM4', # tick.Symbol().decode('utf-8')
            'bid': 5000.25,   # tick.Bid()
            'ask': 5000.50,   # tick.Ask()
            'bid_size': 10,   # tick.BidSize()
            'ask_size': 15,   # tick.AskSize()
            'venue_id': 1     # tick.VenueId()
        }
        
        # Approximate size: 8 + 8 + 8 + 8 + 4 + 4 + 1 ~= 41 bytes per tick in raw format
        row_size = 48 
        
        self.current_batch.append(data_row)
        self.current_batch_size += row_size
        
        if self.current_batch_size >= self.memory_limit_bytes:
            logger.info("Memory ceiling reached. Triggering backpressure & flush.")
            # Trigger backpressure (pause subscription)
            # msg.in_progress() # Signal working
            await self._flush_and_upload()

    async def _flush_and_upload(self):
        """Flushes the current batch to Arrow/Parquet and uploads to GCS."""
        if not self.current_batch:
            return
            
        logger.info(f"Building Arrow Table with {len(self.current_batch)} rows...")
        
        # Convert to arrays
        arrays = []
        for col_name in self.schema.names:
            arrays.append(pa.array([row[col_name] for row in self.current_batch]))
            
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
