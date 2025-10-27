import os, subprocess, signal, time
from typing import Optional

NODE_PROC: Optional[subprocess.Popen] = None

def start_node(env: dict, cwd: str, entry: str) -> int:
    global NODE_PROC
    if NODE_PROC and NODE_PROC.poll() is None:
        return NODE_PROC.pid
    NODE_PROC = subprocess.Popen(
        ["node", entry],
        cwd=cwd,
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        start_new_session=True,  # so we can kill the whole group
    )
    return NODE_PROC.pid

def stop_node(timeout: float = 5.0) -> bool:
    global NODE_PROC
    if not NODE_PROC or NODE_PROC.poll() is not None:
        NODE_PROC = None
        return True
    os.killpg(os.getpgid(NODE_PROC.pid), signal.SIGTERM)
    t0 = time.time()
    while time.time() - t0 < timeout:
        if NODE_PROC.poll() is not None:
            NODE_PROC = None
            return True
        time.sleep(0.1)
    os.killpg(os.getpgid(NODE_PROC.pid), signal.SIGKILL)
    NODE_PROC = None
    return True

def status() -> dict:
    global NODE_PROC
    return {
        "running": NODE_PROC is not None and NODE_PROC.poll() is None,
        "pid": None if not NODE_PROC else NODE_PROC.pid,
        "returncode": None if not NODE_PROC else NODE_PROC.returncode,
    }
