
import sys
import os
import logging
import threading

# Ensure extensions/python/wrappers is in path
sys.path.append('extensions/python/wrappers')

import databento

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def main():
    logger.info("Verifying Databento wrapper...")
    
    # Test Enums
    logger.info(f"Schema.Trades: {databento.Schema.Trades}")
    logger.info(f"SType.RawSymbol: {databento.SType.RawSymbol}")
    
    # Test Builder
    key = os.environ.get("DATABENTO_API_KEY", "db-kf8GXBjGBTQv84YvK5yUHQNsuJcWY")
    logger.info(f"Building client with key: {key[:4]}...")
    
    try:
        client = databento.Historical.Builder()\
            .set_key(key)\
            .build()
        logger.info("Client built successfully.")
        
        # Define callback
        def record_callback(record):
            rtype = record.rtype()
            # logger.info(f"Record received. RType: {rtype}")
            
            if rtype == databento.RType.Trades: # or Mbp0
                trade = record.get_trade()
                logger.info(f"TRADE: {trade.price} sz={trade.size} side={trade.side}")
            elif rtype == databento.RType.Mbo:
                mbo = record.get_mbo()
                logger.info(f"MBO: {mbo.price} sz={mbo.size} order={mbo.order_id}")
                
        # Test Historical Request (Short range)
        # Note: This might fail if key is invalid or dataset not accessible, but binding integrity is what we test
        logger.info("Requesting data...")
        client.timeseries_get_range(
            dataset="GLBX.MDP3",
            start="2024-01-05T12:00:00",
            end="2024-01-05T12:01:00",
            symbols=["ES.c.0"], # Continuous front month
            schema=databento.Schema.Trades,
            callback=record_callback
        )
        logger.info("Request complete.")
        
    except Exception as e:
        logger.error(f"Error during verification: {e}")

if __name__ == "__main__":
    main()
