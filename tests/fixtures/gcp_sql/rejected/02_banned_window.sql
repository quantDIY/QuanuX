SELECT 
    timestamp_ns, 
    bid_price,
    AVG(bid_price) OVER (PARTITION BY instrument_id ORDER BY timestamp_ns) as rolling_avg
FROM MarketTick
