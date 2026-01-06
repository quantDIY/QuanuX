import asyncio
import os
import signal
import sys
import logging
import json
from aiohttp import web
import zmq.asyncio
import async_rithmic

# Configure Logger
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger("rithmic-bridge")

# Env / Config
PORT = int(os.getenv("QUANUX_BRIDGE_PORT", 8078))
ZMQ_PUB_PORT = int(os.getenv("QUANUX_ZMQ_PUB_PORT", 5557))
RITHMIC_USER = os.getenv("QUANUX_RITHMIC_USER")
RITHMIC_PASS = os.getenv("QUANUX_RITHMIC_PASS")
RITHMIC_SYSTEM = os.getenv("QUANUX_RITHMIC_SYSTEM", "Rithmic Paper Trading")
RITHMIC_SERVER = os.getenv("QUANUX_RITHMIC_SERVER", "Chicago")

# Globals
routes = web.RouteTableDef()
zmq_ctx = zmq.asyncio.Context()
pub_socket = None
rithmic_client = None

async def setup_zmq():
    global pub_socket
    pub_socket = zmq_ctx.socket(zmq.PUB)
    pub_socket.bind(f"tcp://*:{ZMQ_PUB_PORT}")
    logger.info(f"ZMQ Publisher bound to tcp://*:{ZMQ_PUB_PORT}")

async def broadcast_tick(tick):
    """Broadcast tick data via ZMQ and Log"""
    try:
        data = {
            "type": "tick",
            "symbol": tick.ticker,
            "price": tick.last_price,
            "volume": tick.volume,
            "timestamp": tick.timestamp.isoformat()
        }
        await pub_socket.send_json(data)
    except Exception as e:
        logger.error(f"Failed to broadcast tick: {e}")

async def rithmic_loop():
    """Main Rithmic Connection Loop"""
    global rithmic_client
    
    if not RITHMIC_USER or not RITHMIC_PASS:
        logger.warning("Rithmic credentials not found. Bridge running in IDLE mode (Health Only).")
        return

    # Additional Config
    RITHMIC_URL = os.getenv("QUANUX_RITHMIC_URL", "wss://rituz00100.rithmic.com:443")
    APP_NAME = os.getenv("QUANUX_RITHMIC_APP_NAME", "QuanuX")
    APP_VERSION = os.getenv("QUANUX_RITHMIC_APP_VERSION", "17.9.0.0")

    rithmic_client = async_rithmic.RithmicClient(
        url=RITHMIC_URL,
        user=RITHMIC_USER,
        password=RITHMIC_PASS,
        app_name=APP_NAME,
        app_version=APP_VERSION,
        system_name=RITHMIC_SYSTEM,
    )

    try:
        await rithmic_client.connect()
        logger.info("✅ Connected to Rithmic Plants!")

        # Example Subscription (Dynamic later)
        # await rithmic_client.market_data.subscribe(
        #     exchange="CME", 
        #     symbol="ESZ4", 
        #     callback=broadcast_tick
        # )

        # Keep alive
        while True:
            await asyncio.sleep(1)

    except Exception as e:
        logger.error(f"Rithmic Connection Error: {e}")
    finally:
        if rithmic_client:
            await rithmic_client.disconnect()

@routes.get('/health')
async def health_check(request):
    status = "CONNECTED" if rithmic_client and rithmic_client.connected else "IDLE"
    return web.json_response({"status": "ok", "service": "rithmic-bridge", "connection": status})

@routes.get('/bridge/rithmic/ws')
async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)
    # TODO: Implement Multi-client WS Fallback logic here
    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            await ws.send_str("Ack")
    return ws

async def init_app():
    app = web.Application()
    app.add_routes(routes)
    return app

async def main():
    await setup_zmq()
    
    # Start Web Server (Health + WS)
    runner = web.AppRunner(await init_app())
    await runner.setup()
    site = web.TCPSite(runner, '0.0.0.0', PORT)
    await site.start()
    logger.info(f"HTTP/WS Server started on port {PORT}")

    # Start Rithmic Loop
    await rithmic_loop()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
