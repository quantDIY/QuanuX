SELECT 
    t1.instrument_id,
    t1.bid_price,
    t2.ask_price
FROM MarketTick t1
INNER JOIN MarketTick t2 
    ON t1.instrument_id = t2.instrument_id
WHERE t1.level = 1 AND t2.level = 2
LIMIT 50
