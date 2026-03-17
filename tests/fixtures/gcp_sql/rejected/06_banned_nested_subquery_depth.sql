SELECT instrument_id
FROM MarketTick
WHERE level IN (
    SELECT level 
    FROM MarketTick 
    WHERE instrument_id IN (
        SELECT instrument_id 
        FROM MarketTick 
        WHERE bid_price > 100
    )
)
