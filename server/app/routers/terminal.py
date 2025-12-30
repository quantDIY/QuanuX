
import asyncio
import json
import os
import pty
import select
import struct
import fcntl
import termios
import signal
from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends
from typing import Optional

router = APIRouter(prefix="/api/terminal", tags=["terminal"])

async def verify_local_or_token(websocket: WebSocket, token: Optional[str] = None):
    # For now, we trust connections but this is where we'd validate the session
    # e.g., if websocket.client.host != "127.0.0.1" and not token: close()
    # For this MVP, we just log access.
    # print(f"Terminal Access Request from {websocket.client.host}")
    return True

@router.websocket("/ws")
async def terminal_ws(websocket: WebSocket):
    # In production, use: ws_auth: bool = Depends(verify_local_or_token)
    # But Depends on WebSocket is tricky; easier to call inside.
    await verify_local_or_token(websocket)

    await websocket.accept()
    
    # Spawn PTY
    # We use 'bash' or 'zsh' if available
    shell = os.environ.get("SHELL", "/bin/bash")
    
    # master_fd: file descriptor for the PTY master (to read/write from python)
    # slave_fd: file descriptor for the PTY slave (given to the child process)
    master_fd, slave_fd = pty.openpty()
    
    pid = os.fork()
    
    if pid == 0:
        # Child process: Set as controlling terminal
        os.setsid()
        os.dup2(slave_fd, 0) # stdin
        os.dup2(slave_fd, 1) # stdout
        os.dup2(slave_fd, 2) # stderr
        os.close(master_fd)
        os.close(slave_fd)
        
        # Execute shell
        # Set minimal env
        env = os.environ.copy()
        env["TERM"] = "xterm-256color"
        
        os.execvpe(shell, [shell], env)
    else:
        # Parent process
        os.close(slave_fd)
        
        loop = asyncio.get_running_loop()
        
        # Async Reader for PTY
        async def pty_reader():
             while True:
                 try:
                     # Non-blocking read would be ideal, but select or run_in_executor is easier
                     # We can use loop.add_reader but handling cleanup is tricky inside async func
                     # Let's use run_in_executor with a simple read for MVP or select
                     
                     # Check if fd is ready
                     # r, w, e = await loop.run_in_executor(None, select.select, [master_fd], [], [], 0.1)
                     # if master_fd in r:
                     #     data = os.read(master_fd, 1024)
                     #     if not data: break
                     #     await websocket.send_text(data.decode(errors='ignore'))
                     
                     # Better approach:
                     data = await loop.run_in_executor(None, os.read, master_fd, 10240)
                     if not data:
                         break
                     
                     # Send text or binary? xterm.js handles strings well
                     await websocket.send_text(data.decode(errors='replace'))
                 except OSError:
                     break
                 except Exception as e:
                     print(f"PTY Read Error: {e}")
                     break
        
        reader_task = asyncio.create_task(pty_reader())
        
        try:
            while True:
                message = await websocket.receive_text()
                payload = json.loads(message)
                
                msg_type = payload.get("type")
                
                if msg_type == "input":
                    data = payload.get("data", "")
                    os.write(master_fd, data.encode())
                    
                elif msg_type == "resize":
                    cols = payload.get("cols", 80)
                    rows = payload.get("rows", 24)
                    # Set Window Size
                    winsize = struct.pack("HHHH", rows, cols, 0, 0)
                    fcntl.ioctl(master_fd, termios.TIOCSWINSZ, winsize)
                    
        except WebSocketDisconnect:
            pass
        except Exception as e:
            print(f"WS Error: {e}")
        finally:
            # Cleanup
            reader_task.cancel()
            os.close(master_fd)
            # Kill child
            try:
                os.kill(pid, signal.SIGTERM)
                os.waitpid(pid, 0)
            except Exception:
                pass
