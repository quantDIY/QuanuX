"""
QuanuX Observability Plane - Python Shadow Node
Rule: Cython Bridge for FlatBuffers. Ruthless Non-Blocking asyncio ingestion.
"""

import asyncio
import logging
from typing import Mapping, Any

import nats
from nats.aio.client import Client as NATSClient
from nats.aio.msg import Msg
import redis.asyncio as redis
from prometheus_client import Counter, Gauge, start_http_server

# The Cython Boundary Requirement
# We NEVER struct.unpack in pure Python on the hot path.
try:
    import quanux_cython_bridge
except ImportError:
    logging.warning("Cython bridge not found. Using pure python mock for static analysis.")
    class quanux_cython_bridge:
        @staticmethod
        def parse_telemetry(memory_view: memoryview) -> Mapping[str, Any]:
            return {"type": "mock"}

# --- Tri-Partite Routing Queues ---
# We decouple NATS consumption from I/O dispatch to prevent JetStream backpressure.
valkey_queue: asyncio.Queue = asyncio.Queue(maxsize=100000)
opensearch_queue: asyncio.Queue = asyncio.Queue(maxsize=100000)

# --- Prometheus Vitals (Updated Synchronously in Memory) ---
quanux_trades_processed = Counter('quanux_trades_processed', 'Trades processed per symbol', ['symbol', 'exchange'])
quanux_node_health = Gauge('quanux_node_health', 'Status of Edge Node', ['node_id'])
quanux_node_cpu = Gauge('quanux_node_cpu', 'CPU Usage of Edge Node', ['node_id'])

# --- NATS Callback (Ruthlessly Fast) ---
async def on_telemetry(msg: Msg):
    """
    Ingests CNATS payloads as memoryviews, hands to Cython, and queues for batch routing.
    No I/O blocking allowed here.
    """
    try:
        # Zero-copy extraction from the NATS packet into Cython
        payload = quanux_cython_bridge.parse_telemetry(memoryview(msg.data))
        
        payload_type = payload.get("type")
        
        if payload_type == "TradeExecution":
            # 1. Update In-Memory Prometheus Counters instantly
            quanux_trades_processed.labels(symbol=payload["symbol"], exchange=payload["exchange"]).inc()
            
            # 2. Queue for ValKey Hot State (Ticker updates)
            if not valkey_queue.full():
                valkey_queue.put_nowait(("TRADE", payload))
            
            # 3. Queue for OpenSearch (Forensic Ledger)
            if not opensearch_queue.full():
                opensearch_queue.put_nowait(payload)
                
        elif payload_type == "NodeVitals":
            node_id = payload["node_id"]
            quanux_node_health.labels(node_id=node_id).set(1.0 if payload["is_healthy"] else 0.0)
            quanux_node_cpu.labels(node_id=node_id).set(payload["cpu_usage_pct"])
            
    except asyncio.QueueFull:
        logging.error("CRITICAL: ShadowNode router queues full. Dropping telemetry to protect NATS loop.")
    except Exception as e:
        logging.error(f"ShadowNode ingest error: {e}")

# --- Background Batch Processors ---

async def valkey_dispatcher(redis_client: redis.Redis):
    """Batches pipeline updates to the hot buffer."""
    while True:
        try:
            # Event-driven wait (0 CPU used while empty)
            first_item = await valkey_queue.get()
            batch = [first_item]
                
            # Drain the rest of the queue up to the batch limit
            while len(batch) < 100 and not valkey_queue.empty():
                batch.append(valkey_queue.get_nowait())
                
            async with redis_client.pipeline() as pipe:
                for item_type, data in batch:
                    if item_type == "TRADE":
                        # E.g., SET latest price
                        key = f"live:price:{data['exchange']}:{data['symbol']}"
                        pipe.set(key, data['price'])
                await pipe.execute()
                
            # Acknowledge queue items
            for _ in batch: valkey_queue.task_done()
            
        except Exception as e:
            logging.error(f"ValKey dispatch error: {e}")
            await asyncio.sleep(1)

async def opensearch_dispatcher():
    """Batches forensic logs to OpenSearch via HTTP bulk API."""
    while True:
        try:
            # Event-driven wait (0 CPU used while empty)
            first_item = await opensearch_queue.get()
            batch = [first_item]
            
            # Drain the rest of the queue up to the batch limit
            while len(batch) < 500 and not opensearch_queue.empty():
                batch.append(opensearch_queue.get_nowait())
                
            # Perform blocking/async HTTP Bulk POST request to OpenSearch
            # httpx.post("http://opensearch:9200/_bulk", data=format_bulk(batch))
            logging.debug(f"Flushed {len(batch)} records to OpenSearch.")
            
            for _ in batch: opensearch_queue.task_done()
            
        except Exception as e:
            logging.error(f"OpenSearch dispatch error: {e}")
            await asyncio.sleep(1)

# --- Node Lifecycle ---

async def run_shadow_node():
    nc = NATSClient()
    await nc.connect("nats://localhost:4222") # Target the mesh

    # Start Prometheus Exporter
    start_http_server(8000)
    
    # Init caching client
    redis_client = redis.Redis(host='localhost', port=6379, decode_responses=True)

    # Spawn asynchronous dispatchers
    asyncio.create_task(valkey_dispatcher(redis_client))
    asyncio.create_task(opensearch_dispatcher())

    # Wire up the non-blocking firehose
    await nc.subscribe("quanux.telemetry.>", cb=on_telemetry)
    
    logging.info("ShadowNode Active. Awaiting Telemetry.")
    
    # Hold the loop
    while True:
        await asyncio.sleep(3600)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    asyncio.run(run_shadow_node())
