SELECT instrument_id, bid_price 
FROM MarketTick 
WHERE level IN (SELECT level FROM MarketTick WHERE bid_price > 100.0)
ORDER BY bid_price DESC
LIMIT 50
