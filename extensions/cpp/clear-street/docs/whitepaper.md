# 🚀  QuanuX Adapter: Clear Street Integration / OnixS
> **QuanuX Enterprise Architecture** | February 2026

## 📋 Executive Summary
This document details the architecture and capabilities of the new **QuanuX Clear Street Adapter**. Designed for high-frequency trading (HFT) and institutional rigor, this component bridges the gap between QuanuX's modern runtime and Clear Street's prime brokerage execution services.

## 💎 Strategic Value
| Feature | Benefit |
| :--- | :--- |
| **⚡️ Latency Minimized** | Leveraging **OnixS** (with **QuickFIX** fallback) for µs-level execution. |
| **💾 Data Sovereignty** | **L3 Market Recorder** captures strictly ordered tick data into local **DuckDB/Parquet** archives for high-fidelity backtesting. |
| **🛡 Operational Resilience** | **Dynamic Hot-Swap** allows intraday strategy updates without disconnecting, preserving queue priority. |

## 🏗 Architecture Overview

### 1. Dual-Engine Core
The adapter implements an abstraction layer (`IEngine`) that wraps two distinct backends:
*   **OnixS Backend** (Proprietary Injection): Kernel bypass, thread affinity, spin-locks for zero-copy.
*   **QuickFIX Backend** (Open Source): Standard compliant FIX 4.2/4.4 engine for dev/test/fallback.

### 2. Pre-Trade Risk Gate 🚧
A dedicated C++ `RiskEngine` intercepts every `sendOrder` call (< 1µs latency), enforcing hard limits:
*   ✅ Max Order Quantity
*   ✅ Max Position Size
*   ✅ Max Daily Loss (PnL)

### 3. Unified Data Bus 🚌
*   **Ingestion**: Market data normalized & published to **NATS JetStream** (`CNATS`).
*   **Persistence**: Parallel thread writes L1/L2/L3 updates to **DuckDB**.

## 🔐 Architectural Learnings & Security
1.  **Hybrid C++14/20 Bridge**: Developed a "Bridge Library" architecture to isolate legacy QuickFIX (C++14) code while preserving modern C++20 features for the core engine.
2.  **Zero-Trust Credential Management**: Runtime injection via `QUANUX_CLEARSTREET_PASSWORD` env var. **No secrets in `config.ini`**.

## ✅ Production Readiness Checklist
To promote from `VERIFIED` to `INTEGRATED`:
1.  [ ] **Configuration**: Generate `config.ini` from template (Set `SenderCompID`/`TargetCompID`).
2.  [ ] **Credentials**: Export `QUANUX_CLEARSTREET_PASSWORD`.
3.  [ ] **Strategy**: Compile trading logic to `libstrategy.so`.
4.  [ ] **Network**: Ensure firewall permits TCP to Clear Street Gateway.

---
**Conclusion:** The Clear Street Adapter represents a mature, "State of the Art" trading gateway, combining raw speed with institutional safety and observability.
