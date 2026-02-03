# 🧠 Project Status & Brain Dump Staging

**Project Board**: [Road to v0.0.1](https://github.com/orgs/quantDIY/projects/2)

> **Protocol**:
> 1. **Brain Dump**: User adds raw thoughts/requests here or via chat.
> 2. **Staging**: Agent processes thoughts into "Staged Issues" below.
> 3. **Sync**: Agent executes `gh issue create` and moves items to "Active Issues".

## 📥 Staged Issues (Ready for GitHub)
*Items parsed from chat/docs, waiting for user approval to push.*

*(No staged issues yet. Waiting for Brain Dump...)*

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
- [x] [Testing: Confirm functionality of Figma MCP server with QuanuX MCP](https://github.com/quantDIY/QuanuX/issues/2) `label:testing`
- [x] [Architecture: Polyglot Extensions] Established `extensions/{cpp,go,node,python}` structure.
- [x] [Architecture: Cython Migration] Converted `indicators`, `tws_api`, `databento`, `duckdb`, `rithmic`, `topstep` to Cython (7x perf boost).
- [x] [Architecture: Agent Skills Protocol] Implemented `server/skills` and MCP loader.
- [x] [Feature: QuickFIX Engine] Verified C++ Engine + Python Bindings + Go Integration.
- [x] [Feature: DuckDB Ecosystem] C++ & Python Connectors (Databento -> DuckDB), MotherDuck Integration.
- [x] [Feature: Rithmic Integration] Verified RApiPlus Cython Wrapper with Test Coverage.
- [x] [Feature: Topstep Extension] Pure Cython Implementation (No Node.js Bridge). 100% Test Parity. Legacy Python Removed.
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

