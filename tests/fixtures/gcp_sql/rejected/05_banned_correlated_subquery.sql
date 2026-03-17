SELECT t1.instrument_id, t1.bid_price
FROM MarketTick t1
WHERE t1.bid_price > (
    SELECT AVG(t2.bid_price) 
    FROM MarketTick t2 
    WHERE t2.instrument_id = t1.instrument_id
)
