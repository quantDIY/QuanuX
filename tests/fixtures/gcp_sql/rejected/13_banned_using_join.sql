SELECT 
    t1.instrument_id,
    t2.bid_price
FROM MarketTick t1
JOIN MarketTick t2 USING (instrument_id)
