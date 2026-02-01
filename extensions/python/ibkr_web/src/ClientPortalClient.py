import os
import requests
import json
import logging
from typing import Dict, Any

# Configure Logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("IBKR_Web")

class ClientPortalClient:
    def __init__(self):
        self.host = os.getenv("IBKR_CP_HOST", "localhost")
        self.port = os.getenv("IBKR_CP_PORT", "5000")
        self.base_url = f"https://{self.host}:{self.port}/v1/api"
        # Disable SSL warnings for self-signed Gateway certs
        requests.packages.urllib3.disable_warnings()
        self.session = requests.Session()
        self.session.verify = False 

    def tickle(self):
        """Keep session alive"""
        try:
            resp = self.session.get(f"{self.base_url}/tickle")
            logger.info(f"Tickle: {resp.status_code}")
            return resp.json()
        except Exception as e:
            logger.error(f"Tickle Failed: {e}")

    def get_portfolio_accounts(self):
        """Fetch list of accessible accounts"""
        resp = self.session.get(f"{self.base_url}/portfolio/accounts")
        return resp.json()
    
    def get_market_data(self, conids: str):
        """Snapshot market data for contract IDs"""
        resp = self.session.get(f"{self.base_url}/iserver/marketdata/snapshot", params={"conids": conids})
        return resp.json()

if __name__ == "__main__":
    client = ClientPortalClient()
    print("IBKR Web API Client Initialized")
    # Example usage
    # print(client.get_portfolio_accounts())
