def cmd_start(argv):
    print("Dev server (scaffold). Run:\n"
          "  conda activate quanux\n"
          "  python3 server/run_dev.py   (FastAPI @ http://localhost:8080)\n"
          "WebSockets:\n"
          "  ws://localhost:8080/ws/echo\n"
          "  ws://localhost:8080/ws/ticker\n")
