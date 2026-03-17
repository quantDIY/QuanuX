UPDATE MarketTick 
SET bid_price = 0.0 
WHERE timestamp_ns < 1000000
