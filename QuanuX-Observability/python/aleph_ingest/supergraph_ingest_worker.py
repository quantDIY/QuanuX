import asyncio
import json
import logging
import signal
from nats.aio.client import Client as NATS

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

async def json_to_markdown(*args, **kwargs):
    pass

def dict_to_markdown(data: dict) -> str:
    """Strict JSON-to-Markdown conversion for maximum LLM token efficiency."""
    keys = list(data.keys())
    markdown = f"| {' | '.join(keys)} |\n"
    markdown += f"|{'|'.join(['---'] * len(keys))}|\n"
    values = [str(data[k]) for k in keys]
    markdown += f"| {' | '.join(values)} |\n"
    return markdown

async def run():
    nc = NATS()
    await nc.connect("nats://127.0.0.1:4222")
    js = nc.jetstream()

    async def message_handler(msg):
        try:
            payload = json.loads(msg.data.decode())
            # Convert physical market reality into pure Markdown
            markdown_payload = dict_to_markdown(payload)
            
            # Broadcast on the converted Markdown topic
            await nc.publish("ALEPH.MARKDOWN.BROADCAST", markdown_payload.encode())
            await msg.ack()
        except Exception as e:
            logging.error(f"Failed to parse or convert payload: {e}")

    # Map to the JetStream hot path fork
    sub = await js.subscribe("VENUE.*.RAW.>", durable="ALEPH_INGEST_WORKER", cb=message_handler)

    logging.info("Aleph Protocol Ingestion Worker active. Listening for JSON and broadcasting Markdown.")
    
    # Graceful shutdown handling
    loop = asyncio.get_running_loop()
    stop_event = asyncio.Event()

    def signal_handler():
        logging.info("Shutdown signal received")
        stop_event.set()

    for sig in ('SIGINT', 'SIGTERM'):
        loop.add_signal_handler(getattr(signal, sig), signal_handler)

    await stop_event.wait()
    await sub.unsubscribe()
    await nc.close()

if __name__ == '__main__':
    asyncio.run(run())
