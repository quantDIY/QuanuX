# QuanuX: Personal Quantitative Trading Lab
*AI Ready Self Hosted Multi-Node Research, Development, Deployment and Portfolio Managemnt Framework & Platform*

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/) [![Python](https://img.shields.io/badge/Python-3.10+-blue.svg)](https://www.python.org/) [![Rust](https://img.shields.io/badge/Rust-stable-orange.svg)](https://www.rust-lang.org/) [![Cython](https://img.shields.io/badge/Cython-3.0-yellow.svg)](https://cython.org/) [![Go](https://img.shields.io/badge/Go-1.21+-00ADD8.svg)](https://go.dev/)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.0-3178C6.svg)](https://www.typescriptlang.org/) [![Tailwind v4](https://img.shields.io/badge/Tailwind-v4-06B6D4.svg)](https://tailwindcss.com/) [![React](https://img.shields.io/badge/React-18+-61DAFB.svg)](https://react.dev/) [![Vite](https://img.shields.io/badge/Vite-5+-646CFF.svg)](https://vitejs.dev/) [![Anaconda](https://img.shields.io/badge/Anaconda-44A833.svg)](https://www.anaconda.com/)

**QuanuX** a Free Open Source Software initiative (FOSS) intends to become a complete, end-to-end ecosystem for high-frequency execution, AI-driven strategy generation, and institutional-grade observation. Currently, QuanuX has integrations for several brokers and prop firms utilizing Python and C++ APIs, including FIX adapters for qualified clients. Built for those who refuse to be limited by conventional infrastructure, QuanuX bridges the gap between AI-driven research and ultra-low latency execution. From bare-metal C++ cores to reactive cockpits, QuanuX is developing the tools required to compete natively at the edge of the market. It is the absolute Equalizer, demanding nothing but intellect and effort.

Each module is accompanied with detailed **SKILL** files. Connect any AI hosted locally or remote over API via our MCP server -- agents are trained throughout the codebase on the purpose for each module, the workflow design, coding patterns, and how to integrate with other modules and/or extend the codebase. For example, our integration with Figma MCP allows one to design a feature visually using only prompts with Figma Make AI generating frontend React code. Then, import the React design into QuanuX client directories via AI prompts where hooks and mock data calls are rewritten into QuanuX compliant methods and pointed to the actual backend data source in their cluster. 

Nodes are intended to be hosted on Linux servers with cross platform clients and our CLI terminal client 'quanuxctl' allowing users to connect to owned or leased remote servers, where they can then be conditioned into QuanuX nodes -- OS and security customization with RSA (or other) handshake and NATS private network invitation. Once a node is conditioned, it can then be repurposed into a specific node genre (i.e. regular execution, FIX execution, NIC/Solarflare, observation, statistics, etc.). 

This is the foundation of the "Russian Doll" concept. Rather than call this a "block" system where pieces are stacked upon one another, in QuanuX each layer is stacked *inside* one another. The outermost layer is the basic server/networking controller, then via quanuxctl the preferred node type can be deployed within that node i.e. an execution node or statistical engine, etc. This is endlessly customizable, layer by layer, with the core of the Russian Doll 'the baby' always carrying the core purpose of the node, be it a trading strategy, HFT engine, statistical computation engine, audit & reconcilliation, risk observation, backtesting, replay, etc. The baby is protected at the core, all surface receptors perfectly insulated (hence heavy biological reference in documentation).

A great deal of effort has gone into 'bare metal' execution nodes with underlying control of processor cores/threads and increasing effort is being applied in execution nodes intended for NIC/Solarflare Direct Exchange/Broker Access (DMA) which entirely bypass the Linux OS. The purpose for this is typical SCRUM rationale -- do the hardest part first and everything else falls into place from there. That said, most will never need this functionality and much of the more basic order/data functionality (i.e. REST API Python & C++ non-FIX -- those not requiring conformance testing and independent confirmation to trade) is ready to go.

The AI Strategy Lab is a core feature in development. The dream **Python --> Cython --> C++** is becoming a reality. This will allow traders to work in Python which opens up the world of tools most of us use on a daily basis in a language that is easy to be comfortable with and fast for idea iteration. The Anaconda integration allows environments to be tightly managed and serves as a "source of truth" for package management and QuanuX binaries while enabling seamless transition between QuanuX and Machine Learning/AI tools like PyTorch. Launch QuanuX right from Anaconda Navigator when you install the channel! (More details soon...)

QuanuX backtesting software is coming along quickly and is being built around Databento L3 historical datafeed which QuanuX hopes to offer directly very soon at a massively affordable rate versus what you can find anywhere else. QuanuX is also exploring "one click deploy" with DigitalOcean so traders who do not possess their own server cluster infrastructure can let QuanuX and DO take care of it for them -- this is a lot more affordable than you may expect -- and will never feature a markup over DO's stated rates. Although not the same as having a bare metal server connected by a 3' Cat6 cable at the exchange datacenter, being closer to the exchange with 100% uptime and much lower latency should result in edge for prop and retail traders who aren't building strategies based on sub millisecond signals.

This is a large, ambitious project coming very close to its first release. Your support will absolutely help bring QuanuX v0.0.1 LIVE more quickly -- financial via GitHub Sponsors or code contributions, everything helps! 

---

## ⚡ Pillar I: The Ultra-Low Latency Core (59ns Execution)

**QuanuX is built on a physics-first execution model.** 

- **The Neural Singularity (16 TSC Cycles / ~8ns):** (In Development) By stripping away the standard OS routing latency, QuanuX leverages Solarflare EF_VI Hugepage Overlays to achieve a 143 to 16 TSC cycle reduction in market data ingress.
- **Hardware Interlocks:** By utilizing a 64-byte L3-cache "Dead Drop," the **Sovereign Sentinel** (Core 5) provides 23ns risk validation across the CPU cache without OS context switching, while the **Execution Spreader** (Core 3) handles latency-sensitive Alpha generation.
- **The Trace:** See the verified 59ns tick-to-trade flash-crash survival data in `REPORTS/MISSION_CRITICAL_TRACE.md`.
- **The Sovereign Guarantee:** QuanuX ensures Binary Replay Parity via the HDF5-to-Parquet Databento pipeline. Identical code paths ensure backtests perfectly replay live structural executions with zero drift.

## 🧬 Pillar II: The Biological Octave (Architecture)

QuanuX operates under the paradigm of Biological Sovereignty. The core strategy is protected by form-fitting membranes (Organelles), documented in our 8-section manual hierarchy:

1. **`quanux-spreader(1)` (The Metabolic Enzyme)**: The active execution catalyst.
2. **`ritchie-interlock(2)` (The Immune System)**: Hard-coded risk thresholds and Apoptosis (`STATE_HALT`) laws.
3. **`quanux-organelles(3)` (The Ribosomes)**: Extension APIs for growing new membranes.
4. **`quanux-shm(4)` (The Cytoplasm)**: Lock-free, inter-cellular shared memory state.
5. **`quanux-vault(5)` (The Memory Vacuole)**: HDF5 SWMR Journal for exact parity transcription.
6. **`quanux-arena(6)` (The Training Cortex)**: The Neural Sandbox for simulated evolution via Synthetic Entropy.
7. **`quanux-cell(7) & quanux-aether(7)` (The Nucleus & Aether)**: Solarflare EF_VI kernel bypass architecture.
8. **`quanux-sentinel(8)` (The Homeostatic Admin)**: PTP synchronization and Cellular Regeneration routines.

## 🤖 Pillar III: The Strategy Forge & AI Architect

**The Intelligence Layer & Nutrient Layer.**

- **The Anaconda Synapse:** Deep integration with Anaconda (Conda) provides a seamless Python data science workflow. Evolve strategies and analyze HDF5 memory vacuoles in Python, then transfer the DNA to the C++ Core 3 Spreader.
- **AI Strategy Architect:** An integrated AI Strategy Builder translates quant intent in natural language into high-performance, verified C++ and Rust execution logic.
- **Figma-Grade UX:** A professional-grade research cockpit built with Tauri and React. Design UI natively in Figma and let MCP translate design tokens into a sub-millisecond visual response.

## 🔌 Pillar IV: Connectivity & Injectable Sovereignty

**Global Access and Redundant Adapters.**

- **The Universal Translator (Sanctified Graft):** Using `Makefile.onixs`, users can swap the altruistic stub for the proprietary **OnixS Solarflare Binary Order Handler**. This achieves *Linguistic Singularity*—allowing the Native Nucleus to speak directly to the matching engine.
- **TopstepX Native Integration:** A proprietary GraphQL/WebSocket bridge seamlessly supporting the TopstepX prop-firm platform.
- **Institutional Adapters:** Formalized native support for `QuickFIX` and `Databento`. Bridge across brokerages with bit-identical engine parity.

## 🌐 Pillar V: Universal Connectivity & Brokerages

**Trade where you want. QuanuX brings dozens of brokerages and prop firms under one roof.**

### Proprietary Trading Firms
- 🏆 **Topstep** (Native API Integration)
- 🐂 **Apex Trader Funding**
- 🦅 **Elite Trader Funding**
- 💰 **Take Profit Trader**
- 🚀 **OneUp Trader**
- 🐂 **Bulenox**
- 🎓 **Earn2Trade**
- 🎯 **Uprofit**
- ⏱️ **TickTickTrader**
- 🦁 **Leeloo Trading**
- *...and any other firm compatible with Rithmic.*

### Futures Brokerages
- 🏛️ **Interactive Brokers**
- 🌉 **Clear Street**
- 🛡️ **Ironbeam Futures**
- ⚡ **AMP Futures**
- 📉 **Discount Trading**
- 🌊 **Edge Clear**
- 🏎️ **Optimus Futures**
- 🎯 **Cannon Trading**
- 🏦 **Wedbush Futures**
- 🦉 **Tradovate**
- *...and any other brokerage compatible with Rithmic API or ProjectX.*

### 🛠 Workflow & Analysis Integrations
- **n8n**: Automate your trading operations. Trigger alerts, log trades, or manage risk with visual workflows.
- **Sierra Chart**: The professional's choice. We support direct integration for advanced charting and data.
- **QuanuX MCP**: Agent-skills protocol throughout. Bring your own AI API-Key—QuanuX handles the context and saves tokens.

## 🛡️ Pillar VI: Institutional Reliability & Reconciliation

**Mathematical Parity and Safety.**

- **Deterministic Ledgering:** Discover 1:1 transaction reconciliation via the `.qlog` high-speed binary journal.
- **The Ritchie FSM:** A completely deterministic Finite State Machine ensuring predictable recovery from all market failure modes (from `STATE_VOID` to `STATE_RECOVERY`).
- **Signed Deployments:** SHA-256 verification and Git-signed audit trails are strictly enforced for all production deployments.

---

## 👁️ Pillar VII: Operation Panopticon Genesis & The Global Mesh

**The Unified Control, Execution, and Observability Architecture.**

QuanuX operates as a globally distributed, polyglot organism. Absolute separation of concerns is enforced between structural logic, network control, and telemetry observation.

### 1. The Tier 1 Control Plane (The HA Matrix)
- **The Law of Verified Death (STONITH):** "Shoot The Other Node In The Head". QuanuX guarantees that split-brain scenarios are biologically impossible. Before a standby replica assumes control, it actively severs network and power dependencies of the dead primary.
- **Sentinel HA Loop:** Powered by `sentinel.py`, the dynamic failover daemon manages absolute state.
- **JetStream KV Elections:** Real-time, cryptographically-backed leader elections running directly on the NATS JetStream mesh. 

### 2. The Execution Plane (The Sovereign Engine)
- **C++20 Dominance:** The unyielding core engine. All path-critical trade execution, order book logic, and quantitative computations are natively compiled on the explicit **Edge Nodes** using `-O3 -march=native`. Cross-compilation and Docker are strictly forbidden to guarantee deterministic routing.
- **Zero-Copy FlatBuffers:** JSON is strictly forbidden on the execution plane. Memory states are transferred natively via FlatBuffers, utilizing fixed-point integer math (`int64`) for perfect, zero-loss institutional pricing and CPU cache locality.
- **The CNATS Global Mesh:** All inter-node communication is blasted over the CNATS C client without exception.

### 3. The Observability & Data Plane (The Guest Layer)
- **The Decoupled Panopticon:** The architectural control plane is physically severed into two distinct Terraform-provisioned droplets operating exclusively within the `10.10.10.x` VPC:
  - **`panopticon-ledger`**: Operates exclusively as the Forensic Ledger running the Java OpenSearch engine.
  - **`panopticon-buffer`**: Operates as the High-Frequency Hub running Native ValKey, the NATS JetStream Mesh, and the asynchronous Python Shadow Node.
- **The Python Shadow Node:** A non-blocking asynchronous traffic cop. Python nodes are strictly forbidden from touching the core engine state or blocking the NATS ingestion loop.
- **The Cython Bridge:** Execution loops rely on optimized `quanux_cython_bridge` modules to instantly unmarshal raw bytes. Pure Python `struct.unpack()` is disabled.
- **Tri-Partite Decoupling:** Event-driven, CPU-efficient asynchronous batching queues decoupled into three tiers:
  - **ValKey (Hot State):** Live ticker prices and queue depths managed natively on the buffer node.
  - **Prometheus (Vitals):** Instantaneous in-memory node counter and heartbeat aggregation.
  - **OpenSearch (Forensics):** Background asynchronous HTTP logging to the isolated ledger node.
- **The Air-Gapped CLI:** `quanuxctl obs` (built on Typer). A purely stateless remote control. The terminal interface has zero database access—it communicates solely by publishing authenticated intent payloads directly to the CNATS mesh.

### 4. The Quant Vault (Cold Storage & Settlement)
- **C++ DuckDB Settlement Daemons:** DuckDB analytical logic lives explicitly inside the C++ Core using native `duckdb.h` C API queries. The Python library is structurally forbidden.
- **Dynamic Global Epochs:** Market closes are coordinated via the global Exchange Registry running on JetStream. Markets operate on exact localized epochs.
- **Native Parquet Generation:** The C++ Settlement Daemons natively trigger high-speed `COPY TO` commands to flush the hot buffers into partitioned Parquet chunks natively dispatched to S3 cold storage.

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

## 📜 License

QuanuX is licensed under the **GNU Affero General Public License v3.0 (AGPLv3)**.

- You may use, modify, and distribute this software under the terms of the AGPL for non-commercial use.
- Those who wish to host, modify and/or distribute for commercial use, internally or externally, over network or otherwise must     obtain a license from the project owner.

See the `LICENSE` file for full details.

---

## 📡 Appendix: Live Matrix Telemetry (Institutional Receipts)

The physical capabilities of the QuanuX execution and observability planes have been systematically verified on bare metal. Below are the exact, extracted hardware telemetry proofs gathered from the live matrix nodes, demonstrating the rigorous `10.10.x.x` VPC network decoupling and C++ component integration.

### The Observability Buffer (ValKey & NATS)
Executes strictly on `10.10.10.5`. 
* **NATS JetStream** is successfully bound exclusively to the internal VPC and active.
* **ValKey Cache** responds to live PING requests bridging memory states.

```bash
$ systemctl status nats-server
● nats-server.service - NATS JetStream Mesh Node
   Active: active (running)
Listening for client connections on 10.10.10.5:4222
Server is ready

$ valkey-cli ping
PONG
```

### The Institutional Ledger (OpenSearch)
Executes strictly on `10.10.10.6`. Isolated entirely from execution cycles, guaranteeing forensic immutability.
* **Cluster Health**: `Green` and perfectly discovering the internal IP boundary without external exposure.

```json
$ curl -s http://10.10.10.6:9200/_cluster/health
{"cluster_name":"opensearch","status":"green","timed_out":false,"number_of_nodes":1,"active_primary_shards":4}
```

### The Sovereign Engine (C++20 Spreader)
Deployed via `quanuxctl nest` onto `edge-nyc` droplets natively compiled against explicit `-O3 -march=native` parameters.
* **SystemD Daemon**: Flawlessly active, bound to the `habitat.env` contract.

```bash
$ systemctl status quanux-engine
● quanux-engine.service - QuanuX Sovereign Engine (59ns Spreader Nest)
   Active: active (running)
   Main PID: 6699 (quanux_spreader)
Starting QuanuX-Spreader (59ns Dual-Thread Core)...
[Spreader] Connected to NATS DMA pipe.
[Thread 1] Innode Data Pipe Started. Spinning on MARKET.BIN.
[Thread 2] Strategy & FIX Order Entry Started.
```


