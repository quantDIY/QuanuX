import asyncio
import json
import time
from fastapi import APIRouter, WebSocket, WebSocketDisconnect

router = APIRouter()

@router.websocket("/ws/echo")
async def ws_echo(ws: WebSocket):
    await ws.accept()
    try:
        while True:
            msg = await ws.receive_text()
            await ws.send_text(msg)
    except WebSocketDisconnect:
        return

@router.websocket("/ws/ticker")
async def ws_ticker(ws: WebSocket):
    await ws.accept()
    try:
        i = 0
        while True:
            i += 1
            tick = {
                "ts": time.time(),
                "symbol": "DEMO",
                "price": 100.0 + (i % 5),
                "seq": i,
            }
            await ws.send_text(json.dumps(tick))
            await asyncio.sleep(1.0)
    except WebSocketDisconnect:
        return
