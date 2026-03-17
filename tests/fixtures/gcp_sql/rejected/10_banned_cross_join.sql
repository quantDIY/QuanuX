SELECT 
    t1.instrument_id,
    t2.bid_price
FROM MarketTick t1
CROSS JOIN MarketTick t2
