SELECT 
    instrument_id,
    SUM( (SELECT MAX(bid_price) FROM MarketTick WHERE level=1) ) as complex_sum
FROM MarketTick
GROUP BY instrument_id
