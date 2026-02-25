# 🚀 QuanuX: The All-In-One Sovereign Quant Platform

**QuanuX** is a complete, end-to-end ecosystem for high-frequency execution, AI-driven strategy generation, and institutional-grade observation. It is a modular, high-fidelity quant ecosystem designed to eliminate the gap between AI-driven research and ultra-low latency execution. From bare-metal C++ cores to Figma-designed research cockpits, QuanuX provides the tools required to compete at the edge of the market.

> *“I know a couple of guys who are hoping to fire up some big Ambassadors at 11:59.”*

---

## ⚡ Pillar I: The Ultra-Low Latency Core (59ns Execution)

## ⚡ Pillar I: The Ultra-Low Latency Core (Deterministic Survivability)

**QuanuX is built on a physics-first execution model.** 

- **AMP Architecture:** The core is isolated into distinct hardware threads. The **Sovereign Sentinel** (Core 5) handles absolute risk interlocking, while the **Execution Spreader** (Core 3) handles the latency-sensitive Alpha generation.
- **Hardware Interlocks:** By utilizing a 64-byte L3-cache "Dead Drop," the Sentinel can provide 23ns risk validation across the CPU cache without OS context switching, functioning alongside the **HDF5 Vault** as the primary risk and audit layers.
- **The Trace:** See the verified 59ns tick-to-trade flash-crash survival data in [REPORTS/MISSION_CRITICAL_TRACE.md](REPORTS/MISSION_CRITICAL_TRACE.md).
- **The Sovereign Guarantee:** *QuanuX is the only open-source engine with Binary Replay Parity via the HDF5-to-Parquet Databento pipeline.* Identical code paths ensure that backtests perfectly replay live structural executions with zero drift.

## 🤖 Pillar II: The Strategy Forge & AI Architect

**The Intelligence Layer.**

- **AI Strategy Architect:** An integrated AI Strategy Builder that translates complex quant intent in natural language into high-performance, verified C++ and Rust execution logic.
- **Figma-Grade UX:** A professional-grade research cockpit built with Tauri and React/Tailwind v4. Design your UI/UX natively in Figma and let our MCP integration translate design tokens into a sub-millisecond visual response.
- **Visual Prototyping:** Real-time backtesting and visual strategy layout tools that completely remove the friction between concept and live deployment.

## 🔌 Pillar III: Connectivity & Multi-Broker Support

**Global Access and Redundant Adapters.**

- **TopstepX Native Integration:** An exclusive, proprietary GraphQL / WebSocket bridge seamlessly supporting the TopstepX prop-firm platform for instantaneous risk synchronization.
- **Institutional Adapters:** Formalized native support for `QuickFIX`, `OnixS`, and `Databento`. Bridge across multiple brokerages and liquidity providers with bit-identical engine parity.
- **The Tipster Bridge:** A social/signal layer enabling secure, latency-aware signal distribution.

## 📊 Pillar IV: The Observation Suite & The Annex

**Institutional-Grade Observation.**

- **The Web Theater:** A high-fidelity, NATS-streamed audit interface for monitoring your real-time trades globally.
- **The Critic (Remote Annex):** A physically isolated DuckDB audit node, designed to sit on a remote server (e.g., Antarctica) for deep-dive historical forensics without stealing a single CPU cycle from the Execution Spreader.
- **Jacques-Mode:** A switchable UI toggle that morphs the Web Theater into a high-fidelity, baroque visual experience for auditing and executive reporting.

## 🛡️ Pillar V: Institutional Reliability & Reconciliation

**Mathematical Parity and Safety.**

- **Deterministic Ledgering:** Discover 1:1 transaction reconciliation via the `.qlog` high-speed binary journal.
- **The Ritchie FSM:** A completely deterministic Finite State Machine ensuring safe, predictable, and mathematically provable recovery from all market failure modes (from `STATE_VOID` to `STATE_RECOVERY`).
- **Signed Deployments:** SHA-256 verification and Git-signed audit trails are strictly enforced for all production deployment codes.

---

## 🚀 Getting Started

### 1. Prerequisites

- **Python 3.10+** (Required for the server)
- **Node.js v20+** & **pnpm** (Required for the client)
- **Rust & Cargo** (Required for **development** and building the Tauri desktop app)

### 2. Installation (Anaconda - Industry First)

QuanuX is the **first** quantitative trading platform to offer an official **Conda channel**—bridging the gap between Data Science and Quantitative Trading with zero dependency hell.

```bash
conda config --add channels conda-forge
conda config --add channels QuanuX
conda install quanux
```

### 3. Developer Source Installation

```bash
# Clone the development repository
git clone https://github.com/quantdiy/QuanuX.git
cd QuanuX

# Set up Python Virtual Environment
python3 -m venv .venv
source .venv/bin/activate

# Install Dependencies
pip install -r requirements.txt
pnpm install
```

### 4. Configuration

QuanuX uses the OS Keyring to securely store API keys. You can set them up interactively:

```bash
python -m server.cli.setup_secrets
```

### 5. Running the Application

**Terminal 1: Start the Core Server**
```bash
uvicorn server.app.main:app --host 0.0.0.0 --port 8080 --reload
```

**Terminal 2: Launch the Cockpit (Tauri)**
```bash
pnpm -C client/desktop/tauri-app tauri dev
```

---

## 🏛️ The Final Institutional Rebuttal

> *"We have provided the 'Adapters' the industry expects. We have provided the QuickFIX and OnixS connectivity the institutions require. But make no mistake: underneath the 'Aerospace' documentation, we are firing the Big Ambassadors. If you're looking for 'state of the art,' you're looking at it. Bee 🐝 kind, please rewind—unless you've joined the club."*
> 
> — The QuanuX Foundation
