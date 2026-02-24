# 🚀 QuanuX

**QuanuX** is an early-stage, free open-source quantitative research, development, and algorithmic strategy development, trade manaagement & execution platform. ⚡️

This repository represents a **pre-release development snapshot** as the project approaches its first public technical milestone: `v0.0.1-pre`.

---

## ⚠️ Project Status: Pre-Release

QuanuX is currently in **active development** and should be considered **experimental**.

- APIs, schemas, tools, and internal architecture may change
- Features may be incomplete or non-functional
- Documentation is evolving
- Backward compatibility is not guaranteed

This repository is being made public to support early testing, architectural review, experimentation, and visibility as the project approaches its initial release.

It is **not yet intended as a production-ready system**.

---

## 🧪 Versioning

Current development target: `v0.0.1-pre`

Semantic versioning will be introduced once public APIs are extended and stabilized.

---

## 🔥 Platform Features & Highlights

QuanuX developing something truly unique, bringing together the tools to make researching, building and deploying modular quantitative trading strategies easier than ever before. It's a comprehensive ecosystem for quantitative research & development with simple strategy creation, data management, broker integration and strategy execution using the latest in AI and distributed systems.

### 🎨 Design with AI in Figma
**No other trading platform does this.**
Developers can design their own UI/UX directly in **Figma** with the help of their AI assistants. By utilizing our **Model Context Protocol (MCP)** integration, we bridge the gap between design and code. Your AI agent can communicate QuanuX "agent-skills" and design tokens back to Figma via the MCP server, ensuring that every interface you build is beautiful, functional, and 100% **QuanuX compliant**.

### 🧠 Native Embedded AI (Privacy-First)
**Run `ollama serve` and `quanux_node` simultaneously — they connect automatically.**
QuanuX features a **Native C++ AI Connector** built directly into the Execution Engine.
- **Auto-Discovery**: QuanuX automatically detects running local models (Ollama, Llama.cpp, LM Studio) on startup.
- **No API Keys**: Run entirely offline or on an air-gapped trading server.
- **Zero Latency**: Query high-performance models (Llama 3, Mistral) over localhost in microseconds.

### 🔩 Bolt-On Architecture & `quanuxctl`
Flexibility is king. Our modular **bolt-on architecture** allows you to extend the platform effortlessly. Manage everything with **`quanuxctl`**, our powerful CLI tool designed to streamline your workflow, manage extensions, and control your environment with precision.

#### 🤓 Developer Experience
We believe in classic engineering. Full documentation is built right into the terminal.
```bash
man quanuxctl
```
- **QuanuX-Common/**: Shared C++ core library (contains `StrategyInterface` and `OrderBookL3`).
- **QuanuX-Backtesting-Engine/**: High-performance C++20 backtester with Python bindings (`quanux_backtest`).
- **execution-node/**: The live C++ trading engine core.
- **client/**: Frontends (Web, Desktop, Mobile, Tablet).
- **server/**: Python Control Plane, Agents, and API.
- **extensions/**: Language-specific plugins (C++, Python).
### ⚡️ Distributed Execution Nodes (Bare Metal & Remote)
**Unlimited Scale.**
QuanuX introduces a game-changing **Execution Layer**. Spin up unlimited execution nodes on **bare metal servers** anywhere in the world.
- **Colocation Ready**: Deploy nodes right next to exchange servers (e.g., Aurora CME Data Center) for **ultra-low latency**.
- **"Push" Deployment**: Use `quanuxctl` to push your strategies to remote nodes instantly.
- **Microservice Power**: Your strategies run as independent, high-performance Go binaries, decoupled from the core QuanuX server, yet able to be managed by a remote QuanuX client, CLI or via QuanuX server. HFT is possible with this architecture.

### 🐍 Anaconda Distribution (Industry First)
**Bridging the gap between Data Science, Machine Learning, AI and Quantitative Trading.**
QuanuX is the **first** quantitative trading platform to offer an official **Conda channel**.
- **One Command Install**: `conda install quanux`
- **Zero Dependency Hell**: We manage the complex web of Python and Rust dependencies so you don't have to.
- **Cross-Platform**: Seamlessly consistent environments across Windows, Mac, and Linux.

---

## 🤖 AI Agent & Architectural Rationale

QuanuX is built to be understood and extended by both humans and AI agents.
To ensure our foundational architectural rules (such as the 59ns C++ latency path and the 64-byte L1 cache covenant) are preserved, we maintain a set of explicit instructions and skills:

*   **[Immortal Rationale](docs/IMMORTAL_RATIONALE.md)**: The core C++ engine architecture, historical latency decisions, and memory layouts.
*   **[Agent Skills Directory](.agent/skills/)**: High-fidelity AI operating procedures and capabilities logically separated by component domain (`SKILL.md`).
*   **Man Pages**: Operator runbooks logically defined in `server/docs/man`.

Agentic systems evaluating this repository must prioritize reading `docs/IMMORTAL_RATIONALE.md` before embarking on modifications to execution nodes, execution loops, or distributed architectures.

---
## 🔌 Universal Connectivity & Integrations

The "Bolt-On" architecture means we integrate with **everything**.

### 🛠 Workflow & Analysis
- **n8n**: Automate your trading operations. Trigger alerts, log trades, or manage risk with visual workflows.
- **Sierra Chart**: The professional's choice. We support direct integration for advanced charting and data.
- **QuanuX MCP**: MCP everywhere, agent-skills protocol throughout. BYO AI API-Key -- QuanuX guides AI and saves tokens.

---

## 🏦 Supported Brokerages & Prop Firms

Trade where you want. QuanuX brings dozens of brokerages and prop firms under one roof.

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
- 🛡️ **Ironbeam Futures**
- ⚡ **Amp Futures**
- 📉 **Discount Trading**
- 🌊 **Edge Clear**
- 🏎️ **Optimus Futures**
- 🦉 **Tradovate**
- *...and any other brokerage compatible with Rithmic API or ProjectX.*

> **Note**: Rithmic connectivity is provided by the first-class **Rithmic Extension**, managed directly via `quanuxctl`.

---



## 🚀 Getting Started

### 1. Prerequisites

- **Python 3.10+** (Required for the server)
- **Node.js v20+** & **pnpm** (Required for the client)
  - Enable pnpm via: `corepack enable`
- **Rust & Cargo** (Required for **development** and building the Tauri desktop app)
### 2. Installation

#### 🌟 Option A: Stable Release (Recommended)
For the verified, cross-platform release (`v0.0.1-pre`), use Anaconda. This is the **official distribution** hosted by the QuanuX Organization.

1.  **Configure Channels** (once):
    ```bash
    conda config --add channels conda-forge
    conda config --add channels QuanuX
    ```

2.  **Install**:
    ```bash
    conda install quanux
    ```

*This installs the stable application snapshot found at [QuanuX/quanux](https://github.com/QuanuX/quanux).*

#### 🧪 Option B: Active Development (Experimental)
Clone this repository (`quantdiy/QuanuX`) if you want to contribute or access bleeding-edge, experimental features.

> **Warning**: This branch is in active flux and may contain unfinished features.

```bash
# Clone the development repository
git clone https://github.com/quantdiy/QuanuX.git
cd QuanuX

# Set up Python Virtual Environment
python3 -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate

# Install Dependencies
# This installs Python dependencies for the server
pip install -r requirements.txt

# This installs Node dependencies for ALL client packages and server bridges
pnpm install
```

### 3. Development Environment Setup
QuanuX supports a **Polyglot Development Experience**:
*   **VSCode**: Native support via `.vscode` configuration (C++, Python, React).
*   **CLion / Visual Studio**: Standardized `CMakePresets.json` for C++ components.
*   **Xcode**: Helpers to generate `.xcodeproj` files.
*   **Spyder**: Helpers to initialize `.spyproject` configuration.

To configure your preferred IDE, check `server/skills/project_maintenance/SKILL.md` or use the helper scripts in `scripts/`.

### 4. Configuration

QuanuX uses the OS Keyring to securely store API keys.

> **Note for Linux Users:** You may need to install system headers for the keyring backend (e.g., `libsecret-1-dev`, `dbus-x11`, or `gnome-keyring`). macOS and Windows use native system stores.

**Option A: Interactive Setup (Recommended)**

```bash
python -m server.cli.setup_secrets
```

This utility prompts for:
- `TOPSTEP__USERNAME`, `TOPSTEP__PASSWORD`, & `TOPSTEP__API_KEY`
- `OPENAI_API_KEY`
- `QUANUX_GEMINI_API_KEY`
- `TRADINGVIEW_CLIENT_ID`

**Option B: Manual / Additional Keys**

You can also add keys via the **Integrations** page in the app, or manually using Python (e.g., for Gemini):

```bash
# Example for Gemini
# Note: Ensure the key name matches what the application expects (e.g. QUANUX_GEMINI_API_KEY)
python -c "import keyring; keyring.set_password('QuanuX', 'QUANUX_GEMINI_API_KEY', 'your_api_key')"
```

**Option C: Import from .env**

```bash
# Create .env file with your keys (e.g. QUANUX_TOPSTEP__API_KEY=...)
python -m server.cli.import_env
```

### 5. Running the Application

You will need two terminal sessions.

**Terminal 1: Start the Server**
```bash
# Ensure venv is active
source .venv/bin/activate
uvicorn server.app.main:app --host 0.0.0.0 --port 8080 --reload
```

**Terminal 2: Launch Desktop App**
```bash
pnpm -C client/desktop/tauri-app tauri dev
```

### 6. Building for Production (Optional)

If you want to create a standalone executable (e.g., `.dmg`, `.exe`, or `.app`) rather than running in development mode:

```bash
pnpm -C client/desktop/tauri-app tauri build
```

This will output the installers/bundles to `client/desktop/tauri-app/src-tauri/target/release/bundle`.

---

## 📜 License

QuanuX is licensed under the **GNU Affero General Public License v3.0 (AGPLv3)**.

- You may use, modify, and distribute this software under the terms of the AGPL
- If you run a modified version as a network service, you must make the source available

If you wish to use QuanuX in a way that is **not permitted under the AGPLv3**
—for example, offering it as a closed-source or proprietary service—
please contact the project owner to discuss **commercial licensing options**.

See the `LICENSE` file for full details.

---

## 🚧 Contributions

At this stage:

- The project is not yet accepting formal external contributions
- Interfaces and internal boundaries are still settling
- Contribution guidelines will be added once `v0.0.1` stabilizes

Public visibility is intended to support review and exploration, not yet coordinated development.

---

## 🧭 Disclaimer

This software is provided **as-is**, without warranty of any kind.

Nothing in this repository constitutes financial advice, trading advice, or investment recommendations.

---

## 🔭 What’s Next

QuanuX is approaching its first stable technical checkpoint.

Further documentation, examples, and contribution guidelines will follow as the project matures beyond the `v0.0.1` pre-release stage.
