---
description: Localized knowledge base for QuanuX-Observability FlatBuffer schemas.
---

# QuanuX Observability: Schemas Knowledge Base

This document outlines the strict architectural and mathematical laws governing the `QuanuX-Observability/schemas/` directory. These schemas define the exact binary structures used by the C++ execution engine and the Python guest nodes to communicate over CNATS.

## 1. The Floating-Point Sin (Zero Tolerance)
- **Rule**: Never use `double` or `float` for institutional finance values (price, quantity).
- **Reasoning**: Floating-point math creates microscopic rounding errors at the silicon level, which are unacceptable for execution and analytics.
- **Implementation**: We use `int64` (fixed-point integer math with implied decimals) for all pricing and quantities. DuckDB and C++ handle integer arithmetic with perfect precision.

## 2. The String Menace (Cache Locality)
- **Rule**: Minimize or eliminate variable-length strings in high-frequency execution payloads.
- **Reasoning**: FlatBuffers are designed for zero-copy, memory-aligned structs. Strings require the CPU to compute offsets and jump to different memory locations to read null-terminated arrays, thereby destroying CPU cache locality.
- **Implementation**:
  - `trade_id` is a `uint64`.
  - `symbol` is a `uint32` (mapped integer ticker dictionary).
  - `exchange` utilizes the `ExchangeCode` standard enum instead of raw strings where applicable.

## 3. High-Speed Time & Offset Processing
- **Rule**: Avoid string parsing for time-of-day or timezone offsets during critical execution loops or settlement daemons.
- **Reasoning**: Parsing strings like "16:30:00" consumes unnecessary CPU cycles. Offsets mapping to hours (e.g., `int8`) fail globally as they do not account for fractional zones (e.g., India is UTC +5:30).
- **Implementation**:
  - `settlement_close_time` is stored as `uint32` (seconds from local midnight).
  - Timezone offsets use `current_offset_minutes` as an `int16` to appropriately handle exact geographical boundaries without string conversions.

## Polyglot Whole Integration
These schemas act as the ultimate data contract within the QuanuX Mesh.
- **C++ Sovereign Engine**: Compiles these schemas directly, mapping C++ structs to network payloads instantly.
- **CNATS**: Transport medium for distributing these FlatBuffers.
- **Python Guest Nodes**: Cython wraps these unmarshalled payloads for safe observation without risking core engine latency or memory leaks.
