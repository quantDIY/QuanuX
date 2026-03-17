SELECT 
    t1.instrument_id,
    t2.ask_price,
    t3.bid_size
FROM MarketTick t1
JOIN MarketTick t2 ON t1.instrument_id = t2.instrument_id
JOIN MarketTick t3 ON t2.instrument_id = t3.instrument_id
