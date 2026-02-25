# The Sim-Live Parity Protocol (The Final Proof)

The QuanuX ecosystem eliminates "Backtest Drift." We do not simulate the past; we *replay* it natively through the exact same hardware logic paths used in production. 

This is achieved via the integration of the **Sovereign Vault** and the **Databento** ecosystem.

## The Flawless Workflow: From Wire to Vault to Research

The QuanuX architecture guarantees that an execution evaluated offline generates the fundamental parity of a real-time hardware execution. The sequence is absolute:

1.  **The Ingress (NATS)**: `MARKET.BIN` data packets hit the execution node from the live brokerage network.
2.  **The Engine (Core 3)**: The QuanuX-Spreader evaluates the 59ns logic loop.
3.  **The Sentinel (Core 5)**: The Sovereign Sentinel validates the hardware risk interlock in real-time natively bypassing the OS.
4.  **The Vault (HDF5 SWMR)**: The raw 64-byte binary state is "frozen" into an `.h5` file at CPU speeds continuously locking state arrays into persistence. No translation is performed. What occurred on the CPU is what is recorded.
5.  **The Transpiler**: The `.h5` file is parsed offline and projected into a Databento-compatible Parquet schema without destroying the data geometry.
6.  **The Backtester (Databento Handler)**: The structured Parquet file is fed entirely back into the exact same Spreader Engine utilized in live fire.

**The Argument of the Proof:**
If the engine's internal state machine transitions in the backtest match the HDF5-recorded transitions from the live fire to the exact nanosecond pulse, **Sim-Live Parity is 100%**. There is no "simulated drift." There is only the Truth.

## The Algorithmic Binding (SHA-256 Proof)

During live execution, the `quanux-vault` service writes the absolute **SHA-256 hash** of the running Spreader Binary directly into the HDF5 `.h5` root `metadata` header.

When the Replay Engine conducts a forensic backtest, this hash is verified. This mathematically proves to an auditor that the "Blueprint" running in the backward-looking simulation is precisely the exact "Steel" executable that was compiled and triggered during live market conditions.
