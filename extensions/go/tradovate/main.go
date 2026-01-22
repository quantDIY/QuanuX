package main

import (
	"log"
	"os"
	"time"
)

// Tradovate Client (Stub)
// Authenticates with Tradovate API and opens WebSocket.

func main() {
	apiKey := os.Getenv("QUANUX_TRADOVATE_KEY")
	// env := os.Getenv("QUANUX_TRADOVATE_ENV") // Demo/Live

	if apiKey == "" {
		log.Println("Waiting for QUANUX_TRADOVATE_KEY...")
	}

	log.Println("Starting Tradovate Gateway...")

	for {
		time.Sleep(5 * time.Second)
		log.Println("Requesting Access Token (Mock)...")
	}
}
