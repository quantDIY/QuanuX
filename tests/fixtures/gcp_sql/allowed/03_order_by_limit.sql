SELECT timestamp_ns, instrument_id, bid_price
FROM MarketTick
WHERE bid_price > 150.0 
  AND ask_price < 155.0 
  AND level = 1
ORDER BY timestamp_ns DESC
LIMIT 100
