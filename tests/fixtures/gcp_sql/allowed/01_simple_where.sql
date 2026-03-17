SELECT level, bid_price, ask_price 
FROM MarketTick 
WHERE bid_size > 1000 
  AND ask_size > 1000
