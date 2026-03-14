# 🧠 The QuanuX Spine: Ecosystem Architecture & Physical Laws

This document is the absolute Master Spine of the QuanuX Ecosystem. It serves as the unyielding architectural constitution governing all biological metaphors, operational topologies, OS verification baselines, and cybernetic governance laws.

## 🧬 Biological Sovereignty & The "Russian Doll" Architecture

QuanuX operates under the paradigm of **Biological Sovereignty**. The ecosystem is an evolving organism rather than a static codebase. Every operational component is designed based on the **"Russian Doll"** architectural concept. 

Rather than a "block" system where pieces are stacked blindly upon one another, QuanuX layers are stacked *inside* one another to ensure perfect encapsulation and protection. 

The conceptual "Russian Doll" architecture is formally and physically implemented as:
- **Habitat (The Outer Doll / Conditioned Soil):** The physical or virtual OS boundary (e.g., Ubuntu VM, remote server, or bare-metal edge cluster node). The Habitat provides the conditioned runtime soil.
- **Nest (The Core Doll / The "Baby"):** The sovereign logic process deployed deep into the Habitat. The core purpose of the node (a trading strategy, HFT engine, observation layer, protocol bridge) exists as the innermost protected Nest. The "Baby" is guarded at the core, all surface receptors perfectly insulated.

## 🏗️ The 5-Tier Core Topology

To maintain maximum execution speed and organizational sovereignty, QuanuX is strictly partitioned into a 5-Tier layout:

1. **Tier 1: The Server (Command Center)**: Orchestrates Nests and acts as the cerebral gateway. Uses Python (`quanuxctl`, `nats-py`).
2. **Tier 2: Polyglot Nests (Flexibility & Intelligence)**: AI strategy labs and standard API routing. Uses C++, Cython, Python, Golang. Networking relies on CNATS (`libnats-c`) or ZMQ for pure local, single-machine setups.
3. **Tier 3: Bare-Metal Nests (The 59ns Compute Engine)**: Unforgiving high-speed statistics environments on bare-metal Linux. Highly isolated execution. **Python/Anaconda are strictly forbidden.** Networking: CNATS for telemetry; standard Linux kernel (`epoll`, `io_uring`) for execution.
4. **Tier 4: The Fiber Nests (The Kinetic Edge)**: Pure hardware warfare and full high-frequency trading colocation. **Python strictly forbidden.** Networking: Completely abandons standard Linux socket routing in favor of Solarflare network cards leveraging kernel bypass technologies (`ef_vi` raw DMA, or `onload`).
5. **Tier 5: The Global Swarm (Future Horizon)**: Interconnected arbitrage leveraging microwave or millimeter-wave networks between data centers.

## 📜 The AI Governance Law & Cybernetic Immunity

Just as an organism rejects foreign pathogens, QuanuX employs a cybernetic immune system against AI hallucination and structural decay.

- **Decentralized Context Anatomy**: AI ecosystem memory has permanently migrated from JSON files to localized Markdown directives (`*skill.md`) bound organically alongside the specific repositories and modules they govern.
- **The Prime Directive**: [00_PRIME_DIRECTIVE.md](./00_PRIME_DIRECTIVE.md) acts as the ecosystem's cybernetic immune response. It strictly mandates that all AI agents must recursively ingest localized `.md` rules before traversing, modifying, or launching code blocks. Divergence from these codified physical laws is strictly forbidden.

## 🛡️ The Ritchie Protocol (FSM) & Biological Cellular Analogies

The internal defense mechanisms of QuanuX are designed to protect the Nest against catastrophic failure layers.

- **The Ritchie Protocol (FSM):** A completely deterministic Finite State Machine ensuring predictable recovery from all market failure modes (from `STATE_VOID` to `STATE_RECOVERY`). It defines hard-coded risk thresholds and apoptosis laws.
- **The Apoptosis Trigger (`STATE_HALT`)**: If an execution membrane breaches a risk or sanity threshold, the Ritchie FSM instantly invokes cell death (Apoptosis), freezing execution state to prevent kinetic bleeding.
- **The Sentinel Nucleus**: Maintains PTP synchronization and Homeostatic Admin functions spanning the execution mesh.

## ⚙️ The True Sovereign Lifecycle & Deployment Physics

The physical deployment pipeline engineered for Tier 2-4 Edge Nodes ensures Out-of-Band delivery without network contamination.

- **The NATS Handshake**: IPC via port 4222 orchestrates a dynamic Collision Matrix synchronization (dynamic NACK/ACK) for Habitat safety logic.
- **OOB Payload Transfer**: Dedicated background threads explicitly binding to IPv4 to execute Out-of-Band `libcurl` deliveries over the Data Plane. This directly bypasses catastrophic asymmetric NAT routing topologies and IPv6 hypervisor gateways.
- **The Permission Reality**: POSIX `chmod()` integration actively applies `+x` privileges during ingestion to form conditioned execution bounds.
- **Conditioned SSH Wrapper**: Finalized execution mandates happen over a generated `.run_<payload>.sh` script wrapped in strict SSH authority rather than automated system processes.

## 🔒 The Hardware Quarantine (The Affinity Mandate)

Because the L3 sovereign execution caches are vital to physical latency routing, there is azero-tolerance policy on thread drift.

- **The Core 0 Lock**: All dynamically spawned C++ background operations (e.g., asynchronous OOB `libcurl` downloads launched by the Native Envoy daemon) must instantaneously execute `pthread_setaffinity_np` locking themselves to Management Core 0. This restricts background OS jitter and forcefully protects the kinetic trading cores from arbitrary scheduler swaps.

## 🖥️ The OS Verification Mandate

The QuanuX infrastructure network configurations strictly rely on verified physical deployments to prevent AI repetition.

- **UBUNTU 22.04 LTS (Jammy Jellyfish)** serves as the official, unified verification baseline.
- Assorted NAT routing phenomena, VirtualBox UFW port limitations, user-namespace drops by Git (`safe.directory`), and required Linux dependencies (`libssl-dev`) have been rigorously mapped and secured against this precise ecosystem matrix.

## ⚙️ The QuanuX Operational Capabilities & Ecosystem Features

The platform operates as a multi-language colossus spanning institutional trading to artificial intelligence. Below is the exhaustive functional mapping of the ecosystem.

### 1. Broker & Exchange Integrations (The Execution Gateways)
- **The Tri-Pillar IBKR Architecture (`extensions/cpp/ibkr_fix`)**: A specialized Interactive Brokers ecosystem utilizing pure C++ for sub-millisecond execution. It leverages `QuickFIX` for direct Institutional Gateway access alongside native `TWS API` wrappers exposing endpoints via Cython. It includes a `quanuxctl enhance` "Turbo Mode" that recompiles the adapter on the fly using `mimalloc/jemalloc` memory paradigms and asynchronous `spdlog` ring-buffers to eliminate blocking I/O.
- **Pure Cython TopstepX Extension (`extensions/cpp/topstep/cython`)**: The legacy Node.js bridge has been obliterated in favor of a native, memory-safe Cython implementation mapping directly to TopstepX's backend via `SignalR` and REST. This handles live market quotes, limit/stop placement, and integer-safe (`int64`) 2-Billion+ account ID parsings with 100% test parity and massive speed boosts over JavaScript parsing.
- **Rithmic RApiPlus Integration (`extensions/cpp/rithmic/cython`)**: Institutional direct memory access (DMA) mapped flawlessly from C++ to Python. The engine (`PyREngine`) leverages C++ struct mapping into Python dictionaries, maintaining zero-copy speed while exposing high-frequency order callbacks (`trade_print`, `LineInfo`) directly into the agent pipelines.
- **The QuickFIX Engine Component (`extensions/cpp/quickfix`)**: A statically-linked implementation of the open-source industry standard. Adhering to the Immutable Rule 0 (Performance = C++), this module permits the QuanuX execution logic to route institutional Fix4.2/Fix4.4 connections directly through the C++ core without Python overhead.

### 2. Data & Analytics Engines
- **DuckDB C++ Extension (`extensions/cpp/duckdb`)**: The platform's internal state mechanism for large-batch bulk ingestion. This module utilizes the DuckDB amalgamation (`duckdb.cpp`) exposed via minimal Cython wrappers using the pure native `Appender` API. This permits QuanuX to achieve near-instantaneous `INSERT` rates required for ingestion of multi-gigabyte tick data sets locally.
- **Databento Connectors (`extensions/cpp/databento`)**: A pure Cython translation of Databento's C++ SDK. Exposes the `PyHistoricalBuilder` and Client architecture for ingesting raw data directly into strategy memory and backtesting simulations without relying on the Databento Python SDK overlay constraints.
- **QuanuX Indicators (`libquanux-indicators`)**: The master replacement for the aging `TA-Lib` stack. This C++20 header-only library exposes **Lazy Evaluation** logic utilizing native `std::ranges` syntax (e.g., `prices | quanux::indicators::sma(10)`). This prevents the catastrophic memory allocation inherent to standard Python vectors. It features custom Storage Policies (`DenseStorage` arrays for Futures, `SparseStorage` maps for crypto) mapped directly via PyBind/Cython wrappers achieving up to 7x execution acceleration for Python agents.

### 3. The AI Strategy Lab (QuanuX Foundry)
- **The Polyglot Tier 2 Strategy Layer**: The Foundry architecture abandons monoliths for "Sovereign Components". Mathematical operations (Indicators, Entries, Risk) are entirely decoupled and generated based on exact JSON Intermediate Representations (`indicator_ir.json`).
- **The `FoundryGenerator` Pipeline**: Agent interaction routes from natural language into absolute determinism. When prompted, the NATS-enabled FastAPI orchestrator translates the mathematical IR into three separate languages: Python (`asyncio.TaskGroup` optimized), Cython (`cdef` memory layouts), and C++20 (`std::ranges`).
- **The Deterministic Sandbox Validation**: Code is never blindly vended. The QuanuX ecosystem simultaneously mocks and executes the Python, Cython, and C++ variations of a generated strategy against a Databento-simulated OHLCV array. If there is a fractional discrepancy (e.g. C++ outputs `1.045`, Python outputs `1.044`), the AI generation job constitutes a failure. Determinism is absolute.
- **LLM/MCP Web Subscriptions**: Artificial Intelligence interactions (such as with the Gemini CLI Extension) are pushed through standard REST triggers (`/api/foundry/forge`) but immediately streamed via NATS and Strawberry GraphQL back to human operators, providing real-time token generation telemetry over WebSockets.

### 4. The UI/UX & Interactivity (The Presentation Domes)
- **The Distributed Target Matrix (React Native)**: The platform UI spans 7 independent form-factor targets simultaneously (Car, Foldables, Mega, Mobile, Tablet, Vision, Wear). All targets strictly consume an identical core design system via Expo Router and NativeWind v4 (Tailwind v3 compatibility).
- **The Backend-Driven Law**: The React frontend nodes are classified as "Dumb Domes". All computation, mathematical parsing, and state logic remains completely isolated on the Rust/Python backends. The UI uses the "Beast Mode" Ref-Buffer (`useRef` + `requestAnimationFrame`) to bypass React re-render penalties, painting raw array data directly onto the DOM without halting the user interface.
- **The Figma Developer MCP Server**: The ultimate Design-to-Code loop. The QuanuX MCP orchestrator inspects active Figma designs via `.figma_tokens`, identifies underlying layout math, and programmatically spawns standard `Shadcn/UI` Tailwind-driven code blocks matching the literal design specification while adhering to the Frontend-Standards mandate.
- **The `quanuxctl` Typer Interface**: The C&C Python CLI. Providing exhaustive operational power from creating Topstep keys in the OS Keychain, initiating `.so` strategy deployments, to publishing distributed telemetry stop/start signals over NATS IPC to shut down failing Tier 4 nodes remotely.

### 5. The QuanuX-Annex Ingestion Engine
- **HA Zero-Allocation NATS Core (`QuanuX-Annex/`)**: A sovereign C++ telemetry ingestion daemon deployed via Ansible to the outer Habitats. It maps high-velocity NATS JetStream data directly into byte-aligned C++ structs (e.g. `MarketTick`, `ExecutionLog`) using `<nats/nats.h>`, bypassing JVM and Python garbage collection completely.
- **The Sentinel Protocol Pivot**: Incorporates rigorous Red Team mitigations for observability. Telegraf is deployed via direct static `.deb` injection into the outer droplets, avoiding hypervisor APT repository corruptions, with its telemetry interval dynamically orchestrated via `quanuxctl`.
- **Phase 12 Pipeline Proving Ground (`tests/nats_injector.py`)**: Real-time NATS mocking tools written in Python that map native `struct` byte-alignments to validate the C++ QuanuX-Annex core without requiring a live colocation feed.

## 📂 Repository Anatomy (Topographical Map)

To effectively navigate the ecosystem, the repository is sliced along strict functional boundaries:

- `.agent/`, `server/skills/`, and `extensions/**/SKILL.md`
  - **Purpose**: The Decentralized AI Cybernetic Context.
  - **Integration**: The absolute first point of reading for any AI interaction. This replaces the monolithic `.gemini` AI prompt folder and embeds exact functional truths right beside the executing source code.

- `extensions/`
  - **Purpose**: The C++, Cython, Python, Go, and Node execution modules.
  - **Integration**: Holds the high-throughput Broker Adapters (`quickfix`, `topstep`, `rithmic`, `ibkr`), external Data Engines (`duckdb`, `databento`), and UI integrators (`figma`).

- `src/`, `QuanuX-Clustering/`, and `execution-node/`
  - **Purpose**: Hardware Orchestration and Edge Provisioning.
  - **Integration**: Location of `node_init.sh`, the OS manifest profiles (`tier1_control.yaml` down to `tier4_dma.yaml`), and the pure C++20 `Native Envoy` runtime loops mapping execution to hardware affinity contexts.

- `server/`
  - **Purpose**: The Python Tier 1 Brain.
  - **Integration**: Contains the core REST FastAPI servers, Strawberry GraphQL configurations, the Foundry AI Agent translation loops, the C++ `indicators` engine bridging logic, and the central `quanuxctl` CLI tools.

- `client/`
  - **Purpose**: The Frontend Cross-Platform UX.
  - **Integration**: Contains the React Web, React Desktop (Tauri), and React Native architectures alongside the strict `design-library` mockup lab.

- `QuanuX-Statistics/`, `QuanuX-Backtesting-Engine/`, and `QuanuX-Spreader/`
  - **Purpose**: Low-Latency Quant Mathematics and Routing.
  - **Integration**: The core proprietary computation repositories executing fractional Kelly simulations, event-driven historical tape processing, and the primary 59-nanosecond execution interlocks.
