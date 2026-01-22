package main

import (
	"encoding/json"
	"log"
	"net/http"
	"os"
	"time"
)

// Config
var (
	Port      = "9000"
	BridgeKey = ""
)

func main() {
	// 1. Load Config from Env
	if p := os.Getenv("PORT"); p != "" {
		Port = p
	}
	BridgeKey = os.Getenv("QUANUX_BRIDGE_KEY")

	if BridgeKey == "" {
		log.Println("[WARNING] QUANUX_BRIDGE_KEY is not set. Auth is disabled (Unsafe!)")
	}

	// 2. Setup Router
	mux := http.NewServeMux()

	// Health
	mux.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		json.NewEncoder(w).Encode(map[string]string{
			"status":  "ok",
			"service": "n8n-bridge",
			"time":    time.Now().Format(time.RFC3339),
		})
	})

	// Protected Endpoint (Pilot)
	mux.HandleFunc("/api/info", authMiddleware(func(w http.ResponseWriter, r *http.Request) {
		json.NewEncoder(w).Encode(map[string]interface{}{
			"message": "Hello from QuanuX n8n Bridge (Go)",
			"capabilities": []string{
				"strategy.generate",
				"market.data",
			},
		})
	}))

	// 3. Start Server
	log.Printf("Starting n8n-bridge on port %s...", Port)
	if err := http.ListenAndServe(":"+Port, mux); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}

// authMiddleware enforces the BridgeKey if set
func authMiddleware(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		if BridgeKey != "" {
			apiKey := r.Header.Get("X-API-Key")
			if apiKey != BridgeKey {
				http.Error(w, "Unauthorized", http.StatusUnauthorized)
				return
			}
		}
		next(w, r)
	}
}
