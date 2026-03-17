SELECT 
    level, 
    COUNT(*) as tick_count,
    SUM(bid_size) as total_bid_depth,
    AVG(bid_price) as mean_bid,
    MIN(bid_price) as min_bid,
    MAX(ask_price) as max_ask
FROM MarketTick 
WHERE level <= 5
GROUP BY level
ORDER BY level ASC
