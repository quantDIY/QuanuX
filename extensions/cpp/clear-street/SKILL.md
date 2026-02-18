---
name: clear-street-adapter
description: High-Performance C++ Adapter for Clear Street API (OnixS/QuickFIX Dual Engine)
version: 1.0.0
---

# Clear Street C++ Adapter

This extension provides a production-grade, high-frequency trading gateway to Clear Street.

## Architecture

*   **Dual Engine**: Automatically selects **OnixS** (if injected) or falls back to **QuickFIX**.
*   **Persistence**: **DuckDB** records L2/L3 market data to Parquet for replay.
*   **Safety**: **Pre-Trade Risk Layer** enforces limits *before* orders leave the process.
*   **Agility**: **Hot-Swap** strategies without dropping the FIX session.

## integration

### 1. OnixS Injection (Optional but Recommended)
To enable the high-performance engine:

```bash
quanuxctl integrate onixs --path /path/to/downloaded/onixs-sdk
```

This copies the SDK to `extensions/sdks/onixs`. The build system detects this automatically.

### 2. Configuration (`local_config.json`)

```json
{
  "adapter": {
    "engine": "auto",  // auto, onixs, quickfix
    "sender_comp_id": "MY_ID",
    "target_comp_id": "CST_ID",
    "socket_connect_host": "fix.clearstreet.io",
    "socket_connect_port": 1234
  },
  "risk": {
    "max_order_qty": 50,
    "max_daily_loss": 5000.0
  },
  "recorder": {
    "enabled": true,
    "mode": "L3",  // L1, L2, L3 (Market by Order)
    "directory": "data/archived"
  },
  "performance": {
    "cpu_affinity_send": 4,
    "cpu_affinity_recv": 5,
    "spin_lock": true
  }
}
```

## Usage

### Build
```bash
quanuxctl install clear-street
```

### Run
```bash
quanuxctl run clear-street
```

## Features

### L3 Recorder
Captures full Market-By-Order feeds. Data is stored in DuckDB files optimized for analytical queries.

### Hot-Swap
To reload a strategy:
1. Recompile your strategy shared library (`.so`).
2. Send `SIGHUP` to the adapter process OR use the control command:
   `quanuxctl send clear-street reload_strategy`

### Latency Metrics
Microsecond histograms are logged to `metrics.json` on exit, or streamed via CNATS subject `quanux.metrics.latency`.
