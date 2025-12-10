import time
import subprocess
import requests
import sys
import os
import signal
import socket

def run_server(cmd, port, name):
    print(f"Starting {name} on port {port}...")
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return proc

def wait_for_port(port, timeout=15):
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=1):
                return True
        except (OSError, ConnectionRefusedError):
            time.sleep(1)
    return False

def verify():
    # 1. Start Flask Bridge
    flask_cmd = f"{sys.executable} -m server.bridges.signalr.flask.app"
    flask_proc = run_server(flask_cmd, 8077, "Flask Bridge")
    if not wait_for_port(8077):
        print("Flask Bridge failed to start.")
        if flask_proc: flask_proc.terminate()
        return

    # 2. Start FastAPI
    fastapi_cmd = f"{sys.executable} -m uvicorn server.app.main:app --host 127.0.0.1 --port 8080"
    fastapi_proc = run_server(fastapi_cmd, 8080, "FastAPI")
    if not wait_for_port(8080):
        print("FastAPI failed to start.")
        if flask_proc: flask_proc.terminate()
        if fastapi_proc: fastapi_proc.terminate()
        return

    try:
        # 3. Test Connection
        print("Testing /topstep/connect...")
        payload = {"hub_url": "https://dummy.hub", "access_token": "dummy_token"}
        try:
            resp = requests.post("http://127.0.0.1:8080/topstep/connect", json=payload, timeout=5)
            print(f"Response: {resp.status_code} - {resp.text}")
            
            if resp.status_code == 200 and resp.json().get("ok"):
                print("SUCCESS: FastAPI successfully talked to Flask Bridge.")
            elif resp.status_code == 502:
                print("SUCCESS: FastAPI talked to Flask Bridge (Connection to dummy hub failed as expected).")
            else:
                print("FAILURE: Unexpected response.")
        except Exception as e:
            print(f"FAILURE: Request failed: {e}")

    finally:
        print("Shutting down servers...")
        flask_proc.terminate()
        fastapi_proc.terminate()
        flask_proc.wait()
        fastapi_proc.wait()

if __name__ == "__main__":
    verify()
