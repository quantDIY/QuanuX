SELECT 
    instrument_id, 
    (SELECT MAX(bid_price) FROM MarketTick) as global_max_bid,
    bid_size 
FROM MarketTick
LIMIT 10
