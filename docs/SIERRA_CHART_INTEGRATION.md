# Sierra Chart Integration Guide

**Role**: QuanuX acts as a **DTC Client** connecting to your Sierra Chart instance (DTC Server).

## 1. Overview
Sierra Chart is a high-performance desktop trading platform. QuanuX integrates with it via the **Data and Trading Communications (DTC) Protocol**.

-   **Data Flow**: Sierra Chart -> (DTC) -> QuanuX Extension -> (HTTP/WS) -> QuanuX Core
-   **Execution**: QuanuX Strategy -> (HTTP/WS) -> Extension -> (DTC) -> Sierra Chart

## 2. Configuration (Sierra Chart Side)
1.  Open Sierra Chart.
2.  Go to **Global Settings** -> **Data/Trade Service Settings**.
3.  Click **DTC Protocol Server**.
4.  Enable **Enable DTC Protocol Server**.
5.  Note the **Listening Port** (default `11099` for Historical/Market Data).
6.  Ensure **Encoding** supports JSON (or Binary if implemented).

## 3. Configuration (QuanuX Side)
Go to **Settings** in QuanuX Web/Desktop and set:
-   `Sierra Chart Host`: `localhost` (or remote IP if tunneled).
-   `Sierra Chart DTC Port`: `11099`.
-   `Sierra Chart Bridge Key`: Generate a local key.

## 4. Running the Extension
```bash
cd extensions/sierra-chart
export QUANUX_BRIDGE_KEY=$(quanuxctl secrets get QUANUX_SIERRA_BRIDGE_KEY)
go run main.go
```

## 5. Remote / Cloud Usage
If QuanuX is on the cloud and Sierra Chart is on your PC:
1.  **SSH Tunnel**: `ssh -R 11099:localhost:11099 user@quanux-cloud`
2.  **QuanuX Config**: Set `Sierra Chart Host` to `localhost`.
3.  The Cloud Extension wraps to the local tunnel, which forwards to your PC.
