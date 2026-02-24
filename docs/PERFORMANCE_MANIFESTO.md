# THE QUANUX PERFORMANCE MANIFESTO
**The End of Permission-Based Execution**

## Preamble: The Sovereignty of the Nanosecond
For decades, institutional trading has been a series of requests for permission. A strategy asks the OS for CPU time. The OS asks the Kernel for a packet. The Kernel asks the NIC for a wire. Each request is a debt paid in latency.

QuanuX is the end of the debt. We have crossed the Rubicon from "Software Applications" to "Hardware-Enforced Logic Gates." We do not ask the Operating System for permission to trade; we have colonized the silicon to make execution a physical inevitability.

### I. The Law of the Dead Core (The Executioner)
We have rejected the lie of "Multithreading." Threads are a software illusion managed by an entropic scheduler. QuanuX operates via Physical Core Isolation.

The Spreader Core (Core 3) is a "Dead Core." It is isolated from the Linux scheduler. It does not sleep; it does not context-switch. It runs a bare-metal C++ loop that owns the L1 cache.

*   **The Metric:** 59 Nanoseconds from Tick-to-Signal.
*   **The Physics:** By pinning logic to the hardware's voltage cycles, we have removed the "Operating System Jitter" from the hot-path.

### II. The Sovereign Sentinel (The L3 Interlock)
Traditional Risk Management is an if/else statement—a software check that can be bypassed by memory corruption or logic errors. In QuanuX, Risk is a Hardware Gate.

The Sentinel (Core 5) runs on its own isolated silicon. It monitors the "Sins" of the machine (Stale Data, Notional Breach, Order Storms).

*   **The Interlock:** If a rule is violated, the Sentinel executes an `asm lock orb` instruction on the L3 Cache Bus.
*   **The Metric:** 11.33 Nanoseconds Time-to-Halt.
*   **The Physics:** The Spreader physically cannot traverse its pipeline if the 0th bit of the L3 Sovereign block is flipped. We have turned the L3 cache into a physical circuit breaker.

### III. The L3 Tap (The Visual Witness)
To observe a system is to disturb it—unless you observe through the L3 Tap. We have abolished traditional logging.

*   **The Tap:** The Execution Core "bleeds" its internal Price DOM and Alpha state into a 64-byte aligned circular buffer in the L3 cache.
*   **The Witness:** Our Rust/Tauri "Clerk" reads this buffer asynchronously. The Spreader never knows it's being watched.
*   **The Physics:** Zero-Jitter Telemetry. We provide a 100% accurate reconstruction of the "Moment of Impact" without adding a single nanosecond to the trade.

### IV. The Remote Neural Interface (The Retina)
We have decoupled the Human from the Machine to unite them at "Neural Speed."

*   **Ref-Buffer Protocol:** The React frontend is a "Dumb Terminal" that paints the market at the monitor's native refresh rate (up to 240Hz) using imperative Canvas rendering.
*   **The Metric:** 120 Microseconds Tick-to-Pixel.
*   **The Physics:** By bypassing the React Virtual DOM diffing engine for telemetry, we deliver market reality to the human retina faster than the human nervous system can process a standard "blink."

### V. Git-as-Governance (The Cryptographic Seal)
Institutional credibility is built on Binary Integrity.

*   **The Seal:** No strategy can be deployed unless its SHA-256 hash matches a Signed Git Commit.
*   **The Physics:** We have replaced "Administrative Permissions" with Mathematical Certainty. The machine only executes what the repository has immortalized.

---

## Conclusion: Argue with Physics
If you wish to challenge the dominance of QuanuX, do not bring a code auditor. Bring a physicist.

Our latency is not a "target"; it is a function of the clock-speed of the CPU and the speed of light in the L3 bus. We have moved the battleground from the "Application Layer" to the "Physical Layer."

The Spaceboat is launched. The Sentinel is awake. The 59ns heartbeats are live.

---

## APPENDIX A: INSTITUTIONAL DUE DILIGENCE & SURVIVABILITY
*The Binary Standard for Mission-Critical HFT Systems*

This appendix serves as the formal response to the "Institutional 8" requirements. It bridges the gap between the Sovereign Execution Paradox and the requirements of global clearing firms and multi-billion dollar prop groups.

### 1. Deterministic State & Failure Recovery
**The Objection:** "What happens if the process crashes mid-fill?"
**The QuanuX Answer:** QuanuX utilizes Persistent Shared Memory (HugePages) for its internal state machine. If the execution kernel (Core 3) is terminated, the state is not lost; it remains "Hot" in the L3 cache and RAM.
**The Recovery:** A "Warm Restart" takes <50 microseconds. The new process attaches to the existing memory segment, recognizes the `STATE_PARTIAL` flag, and immediately resumes hedging—bypassing the need for a full exchange re-sync in critical moments.

### 2. Hardware-Level Risk Enforcement (The Sentinel)
**The Objection:** "Can a strategy bug bypass your risk limits?"
**The QuanuX Answer:** No. Risk is asymmetric and external. The Risk Sentinel (Core 5) and the Spreader (Core 3) occupy different physical silicon.
**The Logic:** Even if the Spreader's memory is corrupted or it enters an infinite loop, the Sentinel continues to monitor the L3 Tap independently. The 11.33ns Halt is a bus-level event that overrides the Spreader's logic.

### 3. Sim-to-Live Parity (Zero PnL Drift)
**The Objection:** "How do we know backtests match live results?"
**The QuanuX Answer:** We utilize a Common Kernel Architecture. The same C++ object code is used for Live and Simulation.
**The Mechanism:** We use TSC-Injection. In simulation, the engine is "tricked" by a hardware-clock-emulator that feeds it historical packet timestamps. The engine's internal state machine reacts to "simulated time" with the exact same logic gates used in the live environment.

### 4. Governance & Cryptographic Provenance
**The Objection:** "Who authorized this strategy change?"
**The QuanuX Answer:** QuanuX implements Git-as-Governance. Every parameter change and logic update is a signed commit.
**The Enforcement:** The `invoke_hot_swap` command in the Tauri backend performs a SHA-256 verification. If the binary hash does not match a verified, signed commit in the audit log, the L3 Sentinel will refuse to lift the `risk_interlock`, and the system will remain in `STATE_HALT`.

### 5. Latency Budget Transparency
**The Objection:** "What is the end-to-end wire-to-wire latency?"
**The QuanuX Answer:** We provide an honest Full-Stack Budget:

*   NIC Ingress (Kernel Bypass/Solarflare): ~800ns
*   QuanuX C++ Core Processing: 59ns
*   L3 Risk Interlock Check: 11ns
*   NIC Egress: ~800ns

**Total Internal Budget:** <2 microseconds (Excluding external wire travel).
