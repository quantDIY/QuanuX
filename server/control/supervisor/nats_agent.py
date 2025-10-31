from __future__ import annotations
import asyncio, json, os
from nats.aio.client import Client as NATS  # nats-py 2.x

from server.control.models import BridgeSpec
from server.control.supervisor.process import start, stop, status, list_bridges

NODE = os.getenv("QUANUX_NODE_ID", "local")
NATS_URL = os.getenv("NATS_URL", "nats://127.0.0.1:4222")


async def _reply(nc: NATS, msg, payload: dict):
    if msg.reply:
        await nc.publish(msg.reply, json.dumps(payload).encode())


async def run():
    nc = NATS()
    await nc.connect(servers=[NATS_URL])

    async def on_up(msg):
        req = json.loads(msg.data.decode() or "{}")
        spec = BridgeSpec(
            name=req.get("name", "signalr"),
            runtime=req.get("runtime", "flask"),
            port=int(req.get("port", 8077)),
            env=req.get("env", {}) or {},
        )
        st = start(spec, node=NODE)
        await _reply(nc, msg, {"ok": True, "status": st.to_dict()})

    async def on_down(msg):
        req = json.loads(msg.data.decode() or "{}")
        st = stop(req.get("name", "signalr"), node=NODE)
        await _reply(nc, msg, {"ok": True, "status": st.to_dict()})

    async def on_status(msg):
        req = json.loads(msg.data.decode() or "{}")
        st = status(req.get("name", "signalr"), node=NODE)
        await _reply(nc, msg, {"ok": True, "status": st.to_dict()})

    async def on_list(msg):
        out = list_bridges(node=NODE)
        await _reply(nc, msg, {"ok": True, "bridges": out})

    await nc.subscribe(f"control.{NODE}.bridge.up", cb=on_up)
    await nc.subscribe(f"control.{NODE}.bridge.down", cb=on_down)
    await nc.subscribe(f"control.{NODE}.bridge.status", cb=on_status)
    await nc.subscribe(f"control.{NODE}.bridge.list", cb=on_list)

    print(f"[control-agent] listening on control.{NODE}.bridge.* @ {NATS_URL}")
    try:
        while True:
            await asyncio.sleep(3600)
    finally:
        await nc.drain()


if __name__ == "__main__":
    asyncio.run(run())
