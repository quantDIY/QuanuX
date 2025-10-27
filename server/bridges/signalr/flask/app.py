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

@app.get("/health")
def health() -> Any:
    return jsonify({
        "service": BRIDGE_NAME,
        "status": "ok",
        "port": BRIDGE_PORT,
        "runtime": "flask",
        "message": "SignalR bridge scaffold alive",
    })

@app.post("/connect")
def connect() -> Any:
    """
    BODY (sample):
    {
      "hub_url": "https://example.signalr/hub",
      "access_token": "token or null",
      "headers": {"X-Custom": "v"},
      "channels": ["ticker", "orders"],  # semantic; hub methods later
      "metadata": {"source": "QuanuX"}
    }
    """
    payload: Dict[str, Any] = request.get_json(force=True, silent=True) or {}
    # TODO: validate & establish connection; store handle in in-memory registry.
    app.logger.info("connect(): received payload=%s", json.dumps(payload)[:400])
    return jsonify({"ok": True, "note": "scaffold only; no real hub yet"})

@app.post("/subscribe")
def subscribe() -> Any:
    """
    BODY (sample):
    {
      "channel": "ticker",
      "symbols": ["ESZ4", "NQZ4"],
      "throttle_ms": 100
    }
    """
    payload: Dict[str, Any] = request.get_json(force=True, silent=True) or {}
    app.logger.info("subscribe(): %s", json.dumps(payload)[:400])
    # TODO: map to hub method (e.g., connection.on("Ticker"), invoke("Subscribe", ...))
    return jsonify({"ok": True, "note": "scaffold; no live subscription yet"})

@app.post("/disconnect")
def disconnect() -> Any:
    """
    BODY (sample): { "connection_id": "abc123" }
    """
    payload: Dict[str, Any] = request.get_json(force=True, silent=True) or {}
    app.logger.info("disconnect(): %s", json.dumps(payload)[:400])
    # TODO: locate connection handle & close
    return jsonify({"ok": True, "note": "scaffold; nothing closed"})

if __name__ == "__main__":
    # Dev-only run (CLI supervisor will normally start this)
    app.run(host="127.0.0.1", port=BRIDGE_PORT, debug=True)
