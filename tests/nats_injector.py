#!/usr/bin/env python3
import struct
import time
import asyncio
from nats.aio.client import Client as NATS
import argparse
import os

async def run(url, subject, msg_type, count):
    nc = NATS()
    try:
        await nc.connect(url)
        print(f"Connected to QuanuX Network via {url}")
        
        for i in range(count):
            if msg_type == "tick":
                # MarketTick struct: uint64_t timestamp_ns, uint32_t instrument_id, double bid_price, double ask_price, uint32_t bid_size, uint32_t ask_size, uint8_t level
                # C-struct byte alignment format: < Q I d d I I B
                data = struct.pack("<QIddIIB", time.time_ns(), 101, 150.25, 150.30, 100, 200, 2)
            elif msg_type == "exec":
                # ExecutionLog struct: uint64_t order_id, uint64_t execution_timestamp_ns, double fill_price, uint32_t fill_quantity, uint32_t strategy_id, uint32_t latency_micros
                # C-struct byte alignment format: < Q Q d I I I
                data = struct.pack("<QQdIII", 5000 + i, time.time_ns(), 150.25, 10, 1, 120)
            elif msg_type == "drift":
                # SchemaDriftEvent struct: uint64_t event_timestamp_ns, uint32_t connection_id, uint32_t anomaly_code, char fix_payload_snippet[128]
                # C-struct byte alignment format: < Q I I 128s
                payload_str = b"8=FIX.4.4\x0135=D\x0199999=ANOMALY"
                data = struct.pack("<QII128s", time.time_ns(), 42, 9999, payload_str)
            else:
                print("Unknown payload type")
                return

            await nc.publish(subject, data)
            if i % 100 == 0:
                print(f"[Burst] Injected packet frame {i}/{count} of {msg_type} to {subject}")
            
            # Throttle slightly to simulate live market velocity
            await asyncio.sleep(0.001)

        print(f"Phase 12 Pipeline Proof Complete: Injected {count} {msg_type} payloads to {subject}")
        await nc.flush()
    except Exception as e:
        print(f"Injection Failed: {e}")
    finally:
        await nc.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="QuanuX Phase 12 - NATS JetStream Mock Injector")
    parser.add_argument("--url", default=os.getenv("QUANUX_NATS_URL", "nats://127.0.0.1:4222"), help="The internal NATS mesh IP to target")
    parser.add_argument("--subject", default="QUANUX.MARKET.TICK", help="The NATS Subject to spray against")
    parser.add_argument("--type", choices=["tick", "exec", "drift"], default="tick", help="The raw C++ struct payload type")
    parser.add_argument("--count", type=int, default=1000, help="Volume of the burst")
    args = parser.parse_args()
    
    asyncio.run(run(args.url, args.subject, args.type, args.count))
