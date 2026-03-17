SELECT 
    t1.instrument_id
FROM MarketTick t1
JOIN (SELECT instrument_id FROM MarketTick WHERE level = 1) t2 
    ON t1.instrument_id = t2.instrument_id
