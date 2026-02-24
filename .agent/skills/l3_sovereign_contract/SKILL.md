---
name: L3 Sovereign Contract Architecture
description: The Primary Lesson for building Hardware-Enforced Logic Gates via Memory-Mapped L3 Cache Interlocks inside QuanuX.
---

# The L3 Sovereign Contract (Primary Lesson)

## Concept
Software is slow. Hardware is absolute. Standard architecture evaluates risk conditions using OS-scheduled threads communicating via condition variables, mutexes, or even ring buffers. At 59ns latency ranges, the act of evaluating risk introduces jitter that invalidates the edge.

## Implementation: The SovereignState

QuanuX introduces the `quanux::SovereignState` paradigm.
1. **The Contract**: A strict `alignas(64)` C++ struct (`64 bytes`), designed to perfectly occupy exactly one physical line in the CPU L3 Cache.
2. **The Components**:
    *   `std::atomic<uint8_t> risk_interlock`: The hardware kill-switch. When `0`, the Spreader is active. When `1`, it is halted.
    *   `telemetry_tap`: A tightly bounded circular buffer (e.g., `L3Snapshot telemetry_tap[3]`) bridging telemetry into the Visual Interface asynchronously without affecting the Execution Core.
3. **The Governance**: Mapped via HugePages (or Intel RDT), the Spreader (Core 3) continuously reads the atomic bitmask (`CMP`). If the Sentinel (Core 5) flips the bit to `1`, the memory cache line synchronizes globally, and the Spreader halts instantly without OS context switching.

## Rules of Engagement
*   **Never modify the 64-byte structural boundary.** Padding must always be calculated (e.g., `uint8_t _pad[X]`) to guarantee `sizeof(SovereignState) == 64`.
*   **Never block inside the Sentinel.** The Sentinel loop updates the L3 contract asynchronously. Doing I/O (like `std::cout`) breaks deterministic scheduling.
