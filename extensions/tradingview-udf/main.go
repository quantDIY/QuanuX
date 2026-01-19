package main

import (
	"log"
	"net/http"
	"os"
)

// TradingView UDF Server (Stub)
// Serves historical data to TradingView Advanced Charts widget.
// Endpoints: /config, /symbols, /search, /history

func main() {
	mux := http.NewServeMux()

	mux.HandleFunc("/config", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Write([]byte(`{"supported_resolutions":["1D","1M"], "supports_search":true}`))
	})

	// TODO: Connect to QuanuX Core DB (DuckDB/ClickHouse) to fetch candles

	port := os.Getenv("PORT")
	if port == "" {
		port = "9002"
	}

	log.Printf("TradingView UDF Provider listening on %s", port)
	log.Fatal(http.ListenAndServe(":"+port, mux))
}
