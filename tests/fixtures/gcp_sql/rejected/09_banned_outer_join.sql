SELECT 
    t1.instrument_id,
    t2.bid_price
FROM MarketTick t1
LEFT OUTER JOIN MarketTick t2 
    ON t1.instrument_id = t2.instrument_id
