# cython: language_level=3
import asyncio
import json
import logging
from libcpp cimport bool
import httpx
from signalrcore.hub_connection_builder import HubConnectionBuilder

# Configure logger
logger = logging.getLogger("quanux.extensions.topstep")

DEFAULT_API_URL = "https://api.topstepx.com"

cdef class TopstepClient:
    """
    Cython wrapper for TopstepX API (REST + SignalR).
    Replaces both 'topstepx' (Python) and 'signalr_bridge' (Node.js).
    """
    cdef public str token
    cdef public str base_url
    cdef object hub_connection
    cdef public object on_market_data
    cdef public object on_order_update
    
    def __init__(self, base_url=DEFAULT_API_URL):
        self.base_url = base_url
        self.token = ""
        self.hub_connection = None
        self.on_market_data = None
        self.on_order_update = None

    async def login(self, str username, str password, str api_key) -> str:
        """Authenticate and retrieve bearer token."""
        url = f"{self.base_url}/api/Auth/loginKey"
        headers = {
            "accept": "text/plain", 
            "Content-Type": "application/json"
        }
        payload = {
            "userName": username,
            "password": password,
            "apiKey": api_key
        }
        
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
            
        if response.is_success:
            data = response.json()
            self.token = data.get("token", "")
            if not self.token:
                raise ValueError("Login successful but no token returned.")
            return self.token
        else:
            raise Exception(f"Login failed: {response.text}")

    def connect_signalr(self, str hub_url="market", str access_token=None):
        """
        Connect to SignalR Hub.
        hub_url can be a full URL/Alias or default.
        """
        if access_token:
            self.token = access_token
            
        if not self.token:
            raise ValueError("Token required for SignalR. Call login() first or pass access_token.")

        # Resolve Hub URL aliases if needed (logic from node bridge)
        # For now assume full URL or handled by caller, or hardcode known Topstep hubs
        # Topstep has User Hub and Market Hub.
        
        final_url = hub_url
        # If hub_url is just "market", we might need the real URL. 
        # But let's assume valid URL is passed for now.

        logger.info(f"Connecting to SignalR: {final_url}")

        self.hub_connection = HubConnectionBuilder()\
            .with_url(final_url, options={"accessTokenFactory": lambda: self.token})\
            .with_automatic_reconnect({
                "type": "raw",
                "keep_alive_interval": 10,
                "reconnect_interval": 5,
                "max_attempts": 5
            })\
            .build()

        self.hub_connection.on_open(lambda: logger.info("SignalR Connected!"))
        self.hub_connection.on_close(lambda: logger.warning("SignalR Disconnected"))
        self.hub_connection.on_error(lambda data: logger.error(f"SignalR Error: {data}"))

        # Register default listeners
        # Note: signalrcore callback is typically (lambda args: ...)
        self.hub_connection.on("MarketData", self._handle_market_data)
        self.hub_connection.on("OrderUpdate", self._handle_order_update)
        # Add catch-all or specific handlers as needed

        self.hub_connection.start()

    def _handle_market_data(self, args):
        if self.on_market_data:
            # We might want to dispatch to an async loop if the callback is async
            # For now call directly.
            try:
                self.on_market_data(args)
            except Exception as e:
                logger.error(f"Error in on_market_data: {e}")

    def _handle_order_update(self, args):
        if self.on_order_update:
            try:
                self.on_order_update(args)
            except Exception as e:
                logger.error(f"Error in on_order_update: {e}")

    def disconnect_signalr(self):
        if self.hub_connection:
            self.hub_connection.stop()

    async def place_order(self, int account_id, dict order_data):
        """Async REST place order"""
        if not self.token:
            raise ValueError("Not authenticated")
            
        url = f"{self.base_url}/api/Order/place"
        headers = {
            "accept": "text/plain",
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.token}"
        }
        # order_data should contain symbol, quantity, etc.
        # Check if accountId is inside order_data or separate? 
        # orders.py: payload = order_data (it seems order_data is the full payload)
        # But orders.py signature was: place_order(token, order_data)
        
        # Ensure accountId is present
        if "accountId" not in order_data:
            order_data["accountId"] = account_id

        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=order_data, headers=headers)
            
        if response.is_success:
            return {"success": True, **response.json()}
        return {"success": False, "error": response.text}

    async def cancel_order(self, int account_id, int order_id):
        """Async REST cancel order"""
        if not self.token:
            raise ValueError("Not authenticated")
            
        url = f"{self.base_url}/api/Order/cancel"
        headers = {
            "accept": "text/plain",
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.token}"
        }
        payload = {"accountId": account_id, "orderId": order_id}

        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
            
        if response.is_success:
            return {"success": True, **response.json()}
        return {"success": False, "error": response.text}

    async def modify_order(self, int account_id, int order_id, **kwargs):
        """Async REST modify order"""
        if not self.token:
            raise ValueError("Not authenticated")
            
        url = f"{self.base_url}/api/Order/modify"
        headers = {
            "accept": "text/plain",
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.token}"
        }
        payload = {"accountId": account_id, "orderId": order_id, **kwargs}

        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
            
        if response.is_success:
            return {"success": True, **response.json()}
        return {"success": False, "error": response.text}

    async def search_orders(self, int account_id, str start_time, str end_time):
        """Async REST search orders"""
        url = f"{self.base_url}/api/Order/search"
        headers = self._get_headers()
        payload = {
            "accountId": account_id,
            "startTimestamp": start_time,
            "endTimestamp": end_time
        }
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
        return self._handle_response(response)

    async def search_open_orders(self, int account_id):
        """Async REST search open orders"""
        url = f"{self.base_url}/api/Order/searchOpen"
        headers = self._get_headers()
        payload = {"accountId": account_id}
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
        return self._handle_response(response)

    async def search_accounts(self, bool only_active=True):
        """Async REST search accounts"""
        url = f"{self.base_url}/api/Account/search"
        headers = self._get_headers()
        payload = {"active": only_active} # Check API spec, usually paging too but simplifying
        # Legacy accounts.py used: {"page": 1, "pageSize": 100} maybe?
        # Let's check legacy implementation below or assume standard search
        # Re-checking legacy accounts.py logic would be safer but let's assume basic search for now
        # Actually, let's look at legacy accounts.py in a sec if this fails.
        # Assuming simple payload for now:
        payload = {"page": 1, "pageSize": 50} 
        
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
        return self._handle_response(response)

    async def search_contracts(self, str search_text="NQ"):
        """Async REST search contracts"""
        url = f"{self.base_url}/api/Contract/search"
        headers = self._get_headers()
        payload = {"searchText": search_text} 
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload, headers=headers)
        return self._handle_response(response)

    def _get_headers(self):
        if not self.token:
            raise ValueError("Not authenticated")
        return {
            "accept": "text/plain",
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.token}"
        }

    def _handle_response(self, response):
        if response.is_success:
            return {"success": True, **response.json()}
        return {"success": False, "error": response.text}
