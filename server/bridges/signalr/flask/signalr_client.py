import logging
import time
import requests
from signalrcore.hub_connection_builder import HubConnectionBuilder

log = logging.getLogger("quanux.signalr.client")

class SignalRClient:
    def __init__(self, hub_url: str, access_token: str = None):
        self.hub_url = hub_url
        self.access_token = access_token
        self.connection = None
        self._handlers = {}
        self._setup_connection()

    def _setup_connection(self):
        builder = HubConnectionBuilder()\
            .with_url(self.hub_url, options={
                "access_token_factory": lambda: self.access_token if self.access_token else ""
            })\
            .configure_logging(logging.DEBUG) \
            .with_automatic_reconnect({
                "type": "raw",
                "keep_alive_interval": 10,
                "reconnect_interval": 5,
                "max_attempts": 5
            })
        
        self.connection = builder.build()
        self.connection.on_open(self._on_open)
        self.connection.on_close(self._on_close)
        self.connection.on_error(self._on_error)

    def _on_open(self):
        log.info("SignalR Connection OPEN")
        # Re-register handlers on reconnect
        for event, callback in self._handlers.items():
            self.connection.on(event, callback)

    def _on_close(self):
        log.info("SignalR Connection CLOSED")

    def _on_error(self, data):
        log.error(f"SignalR Connection ERROR: {data}")

    def start(self):
        if self.connection:
            try:
                log.info(f"Connecting to {self.hub_url}...")
                self.connection.start()
            except Exception as e:
                log.error(f"Failed to start connection: {e}")
                raise

    def stop(self):
        if self.connection:
            try:
                self.connection.stop()
            except Exception as e:
                log.error(f"Error stopping connection: {e}")

    def on(self, event_name, callback):
        self._handlers[event_name] = callback
        if self.connection:
            self.connection.on(event_name, callback)

    def invoke(self, method, *args):
        if self.connection:
            try:
                self.connection.send(method, args)
            except Exception as e:
                log.error(f"Error invoking {method}: {e}")
                raise
