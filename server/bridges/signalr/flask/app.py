from flask import Flask, jsonify, request
from .supervisor import start_node, stop_node, status
import os
from pathlib import Path

app = Flask(__name__)

BASE = Path(__file__).resolve().parents[1]
NODE_DIR = BASE / "node"
ENTRY = str(NODE_DIR / "index.mjs")

# NOTE: secrets should come from our secrets provider later.
def node_env() -> dict:
    env = os.environ.copy()
    for k in ("SIGNALR_HUB_URL", "SIGNALR_ACCESS_TOKEN", "SIGNALR_PROTOCOL"):
        v = os.environ.get(k)
        if v:
            env[k] = v
    return env

@app.get("/bridge/signalr/health")
def health():
    return jsonify({"ok": True})

@app.post("/bridge/signalr/start")
def start():
    pid = start_node(env=node_env(), cwd=str(NODE_DIR), entry=ENTRY)
    return jsonify({"started": True, "pid": pid})

@app.post("/bridge/signalr/stop")
def stop():
    ok = stop_node()
    return jsonify({"stopped": ok})

@app.get("/bridge/signalr/status")
def get_status():
    return jsonify(status())

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8091, debug=True)
