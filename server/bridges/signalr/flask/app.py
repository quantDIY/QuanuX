from __future__ import annotations
import os, json, logging
from typing import Any, Dict
from flask import Flask, request, jsonify

app = Flask(__name__)
log = logging.getLogger("quanux.signalr.flask")
logging.basicConfig(level=os.getenv("QUANUX_LOG_LEVEL", "INFO"))

# --- Config (env-first) ---
BRIDGE_NAME = "signalr-flask"
BRIDGE_PORT = int(os.getenv("QUANUX_BRIDGE_PORT", "8077"))
ALLOW_ORIGINS = os.getenv("QUANUX_BRIDGE_CORS", "*")

# NOTE: Real SignalR plumbing is NOT active yet. This is a scaffold surface.
#       We’ll later add: hub negotiation, access tokens, channel (hub) map, subscriptions, etc.

from .signalr_client import SignalRClient

# Global client instance (for prototype simplicity)
signalr_client: SignalRClient | None = None

@app.get("/health")
def health() -> Any:
    return jsonify({
        "service": BRIDGE_NAME,
        "status": "ok",
        "port": BRIDGE_PORT,
        "runtime": "flask",
        "message": "SignalR bridge scaffold alive",
        "connected": signalr_client.connection.transport.state if signalr_client and signalr_client.connection else "disconnected"
    })

@app.post("/connect")
def connect() -> Any:
    """
    BODY (sample):
    {
      "hub_url": "https://example.signalr/hub",
      "access_token": "token or null"
    }
    """
    global signalr_client
    payload: Dict[str, Any] = request.get_json(force=True, silent=True) or {}
    hub_url = payload.get("hub_url")
    access_token = payload.get("access_token")

    if not hub_url:
        return jsonify({"error": "hub_url required"}), 400

    try:
        if signalr_client:
            signalr_client.stop()
        
        signalr_client = SignalRClient(hub_url, access_token)
        signalr_client.start()
        
        return jsonify({"ok": True, "message": f"Connecting to {hub_url}"})
    except Exception as e:
        log.error(f"Connection failed: {e}")
        return jsonify({"error": str(e)}), 500

@app.post("/subscribe")
def subscribe() -> Any:
    """
    BODY (sample):
    {
      "method": "Subscribe",
      "args": ["ESZ4"]
    }
    """
    global signalr_client
    payload: Dict[str, Any] = request.get_json(force=True, silent=True) or {}
    method = payload.get("method")
    args = payload.get("args", [])

    if not signalr_client or not signalr_client.connection:
         return jsonify({"error": "SignalR not connected"}), 400

    if not method:
        return jsonify({"error": "method required"}), 400

    try:
        signalr_client.invoke(method, *args)
        return jsonify({"ok": True, "message": f"Invoked {method} with {args}"})
    except Exception as e:
        log.error(f"Subscription failed: {e}")
        return jsonify({"error": str(e)}), 500

@app.post("/disconnect")
def disconnect() -> Any:
    global signalr_client
    if signalr_client:
        signalr_client.stop()
        signalr_client = None
    return jsonify({"ok": True, "message": "Disconnected"})

if __name__ == "__main__":
    # Dev-only run (CLI supervisor will normally start this)
    app.run(host="127.0.0.1", port=BRIDGE_PORT, debug=True)
