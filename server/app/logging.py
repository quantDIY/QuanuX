import logging

def setup_logging():
    fmt = "[%(asctime)s] %(levelname)s %(name)s — %(message)s"
    logging.basicConfig(level=logging.INFO, format=fmt)
    logging.getLogger("uvicorn.access").setLevel(logging.WARNING)
