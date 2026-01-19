# Brokerage Integration Guide

This guide covers general brokerage connections including Interactive Brokers (IBKR), Tradovate, and SignalR-based feeds.

## 1. Interactive Brokers (IBKR)
-   **Type**: Bridge to TWS / Gateway.
-   **Location**: `/extensions/ibkr`.
-   **Concept**: QuanuX talks to the local TWS API port (default `7497`). You must have TWS or IB Gateway running.
-   **Config**: Set `QUANUX_IBKR_HOST` and `PORT`.

## 2. Tradovate
-   **Type**: Direct WebSocket API.
-   **Location**: `/extensions/tradovate`.
-   **Concept**: Connects directly to Tradovate's cloud API.
-   **Key**: Requires `QUANUX_TRADOVATE_KEY`.

## 3. SignalR (Prop Firms)
-   **Type**: Generic WebSocket.
-   **Location**: `/extensions/signalr`.
-   **Concept**: Many modern prop firm dashboards use SignalR. This extension acts as a client to scrape/stream data from these dashboards if they don't provide a public API.

## 4. Universal "Bolt-on" Pattern
All these extensions follow the same pattern:
1.  **Configure Config**: Settings -> Integrations.
2.  **Generate Bridge Key**: Allow the sidecar to talk to QuanuX.
3.  **Run**: `go run main.go` (or via `quanuxctl start`).
