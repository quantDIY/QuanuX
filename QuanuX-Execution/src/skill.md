---
description: Local C++ constraints instructing subsequent AI agents regarding RiskKernel modifications.
---

# QuanuX Execution Plane: Hot Path Rules (RiskKernel)

This directory contains the core L1/L2 Execution Path elements (`QuanuX-Execution/src/`). It is subject to extreme latency and mathematical sensitivity.

## 1. The Hydration Gate Doctrine
The `RiskKernel.hpp` operates on two explicit tracks that you may **NEVER** merge.

### 1a. The Hot Track (`check_trade()`)
- This inline function represents the final gate before a packet hits a fiber-optic NIC.
- **Rule**: You are STRICTLY FORBIDDEN from adding external I/O here.
- Any suggestion to query Redis, write to a log file synchronously, interact with `libcurl`, or perform a JetStream `KVStore_Get` inside `check_trade()` is an architectural violation.
- The function must remain `[[nodiscard]]`, `inline`, and `noexcept`. It uses pure integer arithmetic against local class variables (the RAM matrix). 

### 1b. The Cold Track (`hydrate_from_mesh()`)
- This is the initialization barrier. It is designed specifically to block execution via synchronous I/O.
- We deliberately hit the network here (JetStream KV) to ensure that the process cannot fire without perfect consensus validation.
- The private variable `is_hot_` is completely dependent on this function completing cleanly. 

## 2. Hard Limits
The integer constraints (`MAX_ORDER_QTY`) defined as `constexpr` protect the system against configuration tampering via API payloads. Do not abstract these back out into network configurations. They are intended to force a C++ compiler error if violated structurally, protecting capital at the lowest silicon level.
