WITH top_levels AS (
    SELECT level, AVG(bid_price) as avg_bid
    FROM MarketTick
    GROUP BY level
)
SELECT * FROM top_levels WHERE avg_bid > 100
