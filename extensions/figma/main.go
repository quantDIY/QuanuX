package main

import (
	"log"
	"net/http"
	"os"
)

// Figma MCP Server (Stub)
// This service will expose MCP tools to read/write Figma files.

func main() {
	// 1. Config
	apiKey := os.Getenv("QUANUX_FIGMA_KEY")     // User Access Token
	bridgeKey := os.Getenv("QUANUX_BRIDGE_KEY") // Auth to Core

	if apiKey == "" {
		log.Println("Warning: QUANUX_FIGMA_KEY not set. MCP tools will fail.")
	}
	if bridgeKey == "" {
		log.Println("Warning: QUANUX_BRIDGE_KEY not set. Core auth will fail.")
	}

	// 2. Setup Server
	mux := http.NewServeMux()
	mux.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("OK"))
	})

	// TODO: Implement MCP Protocol (JSON-RPC 2.0)
	// mux.HandleFunc("/mcp", mcpHandler)

	// 3. Start
	port := os.Getenv("PORT")
	if port == "" {
		port = "9001"
	}
	log.Printf("Figma MCP Extension listening on %s", port)
	log.Fatal(http.ListenAndServe(":"+port, mux))
}
