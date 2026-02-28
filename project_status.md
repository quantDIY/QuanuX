# 🧠 QuanuX Project Spine & Status

**Project Board**: [Road to v0.0.1](https://github.com/orgs/quantDIY/projects/2)

> **Protocol**: The Spine serves as the absolute master map of the QuanuX Ecosystem history, architectural laws, and active deployment targets. 
> 1. **Brain Dump**: User adds raw thoughts/requests here or via chat.
> 2. **Staging**: Agent processes thoughts into "Staged Issues" below.
> 3. **Sync**: Agent executes `gh issue create` and moves items to "Active Issues".

## 🏗️ The Core Topology (Habitats & Nests)
- **Habitat**: The physical or virtual OS boundary (e.g., Ubuntu VM, bare-metal server). The conditioned soil.
- **Nest**: The sovereign logic process deployed into the Habitat.

**The 5-Tier System Boundary:**
1. **Tier 1: The Server (Command Center)**: Orchestrates Nests. Uses Python (`quanuxctl`, `nats-py`).
2. **Tier 2: Polyglot Nests**: AI strategy labs. Uses C++, Python, Cython, Golang. Networking: CNATS or ZMQ (local).
3. **Tier 3: Bare-Metal Nests (59ns)**: High-speed stats. Bare-metal Linux, isolated cores. Pure C++. Python strictly forbidden. Networking: CNATS telemetry, standard Linux networking execution.
4. **Tier 4: The Fiber Nests**: Pure hardware warfare (Full HFT). Exchange colocation. Pure C++. Python strictly forbidden. Networking: Solarflare EF_VI (raw DMA)/onload.
5. **Tier 5: The Global Swarm**: Interconnected arbitrage (Microwave/mm-wave).

## 📜 The AI Governance Law
- **Decentralized Context**: AI ecosystem memory has migrated from JSON to Markdown (`*skill.md`) across the repository.
- **The Prime Directive**: [00_PRIME_DIRECTIVE.md](./00_PRIME_DIRECTIVE.md) enforces that AI agents must recursively ingest localized `.md` rules before code execution. Divergence from these codified physical laws is strictly forbidden.

## 🧬 The True Sovereign Lifecycle & Patches
The physical deployment pipeline engineered for Tier 2-4 Edge Nodes:
- **The NATS Handshake**: IPC via port 4222 for dynamic Collision Matrix sync (dynamic NACK/ACK).
- **Out-of-Band (OOB) Payload Transfer**: Background threads executing IPv4-bound `libcurl` bypassing asymmetric NAT routing traps.
- **The Permission Reality**: POSIX `chmod()` integration dynamically writing execution capabilities to `.run_<payload>.sh`.
- **The Conditioned SSH Execution Wrapper**: Executing payload over Secure Shell, not automated system calls.
- **CMake FetchContent Isolation**: Complete purge of macOS/Homebrew assumptions, restoring bare-metal Linux purity and bypassing `protoc` cross-compilation errors.
- **Linux Security Execution Blocks**: Bypassed Git `safe.directory` user-namespace drops and linked required `libssl-dev` dependencies natively.

## 🛡️ The Hardware Quarantine (The Affinity Mandate)
- All dynamically spawned C++ threads (e.g., asynchronous OOB `libcurl` downloads in the Native Envoy) must instantaneously execute `pthread_setaffinity_np` locking themselves to Management Core 0. This unconditionally preserves the kinetic trading cores from OS thread-schedule thrashing.

## 🖥️ The OS Verification Mandate
- Staging and deployment physics are strictly bound to the **Ubuntu 22.04 LTS (Jammy Jellyfish)** baseline.
- Assorted NAT routing phenomena, VirtualBox UFW limitations, and Linux kernel execution behaviors have been comprehensively documented against this verified baseline ecosystem.

## 📥 Staged Issues (Ready for GitHub)
*Items parsed from chat/docs, waiting for user approval to push.*

- [ ] [Feature: Visual DevOps Globe (3D Infrastructure Visualization)](https://github.com/quantDIY/QuanuX/issues/NEW) `label:feature,ui,3d`

## 🚧 Active Issues (In Progress)
*Synced from GitHub.*

- [x] [Feature: Implement shorthand aliases for all quanuxctl commands](https://github.com/quantDIY/QuanuX/issues/6) `label:feature`
- [x] [Refactor: Migrate native TopstepX SignalR integration to a standalone Extension](https://github.com/quantDIY/QuanuX/issues/7) `label:refactor`
- [x] [Chore: Prune unused dependencies and package managers (Architecture Cleanup)](https://github.com/quantDIY/QuanuX/issues/8) `label:chore`
- [ ] [Feature: Fork & Extend TA-Lib for QuanuX (Linux-first, C/CPP, Conda-distributable, Python Wrapper)](https://github.com/quantDIY/QuanuX/issues/9) `label:feature`

- [ ] [Testing: Obtain developer login accounts to test Rithmic API broker connections](https://github.com/quantDIY/QuanuX/issues/1) `label:testing`
- [ ] [Feature: Develop SDK for Strategy Builder](https://github.com/quantDIY/QuanuX/issues/3) `label:feature`
- [ ] [Feature: Integrate TradingView](https://github.com/quantDIY/QuanuX/issues/4) `label:feature`
- [ ] [Feature: Integrate Coinbase Pro API](https://github.com/quantDIY/QuanuX/issues/5) `label:feature`

- [ ] [Feature: QuanuX Backtester (C++ Native)](https://github.com/quantDIY/QuanuX/issues/17) `label:feature,big-ticket`
- [x] [Feature: Custom TA-Lib (Linux-First C++ w/ Python Wrappers)](https://github.com/quantDIY/QuanuX/issues/14) `label:feature`
- [ ] [Feature: Strategy Converter (Python to C++)](https://github.com/quantDIY/QuanuX/issues/15) `label:feature`
- [ ] [Feature: IDE Connectors (Jetbrains, VS Code, etc.)](https://github.com/quantDIY/QuanuX/issues/18) `label:feature,dx`
- [ ] [Feature: Native AI Connector (Network/Local Models)](https://github.com/quantDIY/QuanuX/issues/19) `label:feature,ai`
- [ ] [Feature: System Dashboard (quanuxctl)](https://github.com/quantDIY/QuanuX/issues/20) `label:feature,cli`
- [ ] [Feature: VCS Connectors (quanuxctl)](https://github.com/quantDIY/QuanuX/issues/21) `label:feature,cli`
- [ ] [Feature: UI/UX General Enhancements](https://github.com/quantDIY/QuanuX/issues/22) `label:enhancement,ui`
- [x] [Feature: Generate React Native Client Target UIs] (11 Targets Scaffolding Complete) `label:feature,ui,mobile`
- [x] [Documentation: Deep Man Page Updates] `label:documentation`

### Verified / Done
- [x] [Security: Patched Vulnerabilities] Resolved High-Severity ReDoS and Path Traversal alerts in `rollup`, `minimatch`, and `hono` via root workspace `pnpm.overrides`.
- [x] [Infrastructure: Decentralized Codex] Forged `00_PRIME_DIRECTIVE.md` and eradicated all legacy JSON artifacts for semantic Markdown architecture.
- [x] [Testing: Confirm functionality of Figma MCP server with QuanuX MCP](https://github.com/quantDIY/QuanuX/issues/2) `label:testing`
- [x] [Architecture: Polyglot Extensions] Established `extensions/{cpp,go,node,python}` structure.
- [x] [Architecture: Cython Migration] Converted `indicators`, `tws_api`, `databento`, `duckdb`, `rithmic`, `topstep` to Cython (7x perf boost).
- [x] [Architecture: Agent Skills Protocol] Implemented `server/skills` and MCP loader.
- [x] [Feature: QuickFIX Engine] Verified C++ Engine + Python Bindings + Go Integration.
- [x] [Feature: DuckDB Ecosystem] C++ & Python Connectors (Databento -> DuckDB), MotherDuck Integration.
- [x] [Feature: Rithmic Integration] Verified RApiPlus Cython Wrapper with Test Coverage.
- [x] [Security: Remediation] Fixed `protobuf` (recursion), `cryptography` (ECC), and Rust `time` (DoS) alerts.
- [x] [Feature: Topstep Extension] Pure Cython Implementation (No Node.js Bridge). 100% Test Parity. Legacy Python Removed.
- [x] [Feature: Cython Execution Node] High-Performance "Edge" Node Scaffold. Runs `.so` strategies. Supports Direct/Relay Adapters.
- [x] [Feature: Execution Engine] Hybrid Go/C++ Node, Portable/Deployable (Push Deployment).
- [x] [Infrastructure: CNATS] Full C++ NATS Client Integration.
- [x] [API: GraphQL] Strawberry implementation.
- [x] [DevEx: IDE Integrations] Visual Studio Code, JetBrains support.
- [x] [DevEx: Man Pages] `man quanuxctl` documentation system.
- [x] [CLI: Package Management] `quanuxctl` expansion (install/upgrade extensions).
- [x] [Feature: Gemini CLI] Decoupled to `quanux-gemini-cli-mcp`.
- [x] [Security: Hardening] Patched `undici`, `node-tar`, and Rust `unsafe` pointers.
- [x] [Feature: IBKR Ecosystem] Tri-Pillar Architecture (`ibkr_fix`, `tws_api`, `ibkr_web`) + Turbo Mode (`quanuxctl enhance`).
- [x] [Feature: Centralized SDKs] Implemented `extensions/sdks/` repository and `quanuxctl integrate`.

## 💡 Brain Dump Area
*Raw notes. Paste anything here.*

