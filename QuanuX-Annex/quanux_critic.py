import asyncio
import os
import nats
import duckdb
import json

# Le Critique: The Minister of Aesthetic Compliance
# Operating strictly from The Annex.

DB_PATH = "critique_vault.duckdb"
JAQUES_MODE_FLAG = os.path.expanduser("~/.quanux_jaques_mode")

async def main():
    print("[Annex] Booting quanux-critic (Le Critique).")
    
    # Establish DuckDB Vault
    conn = duckdb.connect(DB_PATH)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS audit_log (
            tsc_timestamp BIGINT,
            state INTEGER,
            current_position INTEGER,
            interlock_status INTEGER,
            order_packet VARCHAR
        )
    """)
    print("[Annex] DuckDB Vault established.")

    try:
        nc = await nats.connect("nats://localhost:4222")
        print("[Annex] Connected to NATS.")

        # In a real environment, this might be a JetStream consumer or raw NATS subscriber
        # Headquarters dumps raw JSON/Binary logs to the AUDIT.STREAM and forgets we exist.
        async def message_handler(msg):
            subject = msg.subject
            data = msg.data.decode()
            
            try:
                payload = json.loads(data)
                conn.execute(
                    "INSERT INTO audit_log VALUES (?, ?, ?, ?, ?)",
                    [
                        payload.get('tsc_timestamp', 0),
                        payload.get('state', 0),
                        payload.get('current_position', 0),
                        payload.get('interlock_status', 0),
                        payload.get('order_packet', '')
                    ]
                )
            except Exception as e:
                # Discard cleanly; Le Critique does not care for garbled syntax.
                pass
            
            # The Ping-Back Commentary (Jacques-Logic)
            if os.path.exists(JAQUES_MODE_FLAG):
                commentary = "Received your little data packet. It is quite... industrial. I have filed it in my DuckDB vault. Do try to show more flair next time, Les Incompétents."
                response = {
                    "source": "Le Critique",
                    "commentary": commentary,
                    "event_id": payload.get('event_id', 'unknown')
                }
                await nc.publish("AUDIT.PINGBACK", json.dumps(response).encode())
                print(f"[Le Critique] Dismissed HQ with ping-back.")
            else:
                print(f"[Annex] Ingested record silently.")

        await nc.subscribe("AUDIT.STREAM", cb=message_handler)
        print("[Annex] Spying on AUDIT.STREAM...")

        while True:
            await asyncio.sleep(1)

    except Exception as e:
        print(f"[Annex] Connection failure: {e}")

if __name__ == '__main__':
    asyncio.run(main())
