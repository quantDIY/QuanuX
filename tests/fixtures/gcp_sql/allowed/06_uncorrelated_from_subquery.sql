SELECT t.instrument_id, t.total_depth 
FROM (
    SELECT instrument_id, SUM(bid_size) as total_depth 
    FROM MarketTick 
    WHERE level = 1 
    GROUP BY instrument_id
) t 
WHERE t.total_depth > 5000 
ORDER BY t.total_depth DESC
