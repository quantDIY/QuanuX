# QuanuX

**QuanuX** is an early-stage, open-source quantitative research and trading platform.

This repository represents a **pre-release development snapshot** as the project approaches its first public technical milestone: `v0.0.1-pre`.

---

## ⚠️ Project Status: Pre-Release

QuanuX is currently in **active development** and should be considered **experimental**.

- APIs, schemas, tools, and internal architecture may change
- Features may be incomplete or non-functional
- Documentation is evolving
- Backward compatibility is not guaranteed

This repository is being made public to support early testing, architectural review, experimentation, and visibility as the project approaches its initial release.

It is **not yet a production-ready system**.

---

## 🧪 Versioning

Current development target: `v0.0.1-pre`

Semantic versioning will be introduced once public APIs stabilize.

---

## 🚀 Getting Started

### 1. Prerequisites

- **Python 3.10+** (Required for the server)
- **Node.js v20+** & **pnpm** (Required for the client)
  - Enable pnpm via: `corepack enable`
- **Rust & Cargo** (Required for building the Tauri desktop app)
  - [Install Rust](https://www.rust-lang.org/tools/install)

### 2. Installation

Clone the repository and set up the environment:

```bash
# Clone the repository
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

### 3. Configuration

QuanuX uses the OS Keyring to securely store API keys.

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

### 4. Running the Application

You will need two terminal sessions.

**Terminal 1: Start the Server**
```bash
# Ensure venv is active
source .venv/bin/activate
python -m server.app.main
```

**Terminal 2: Launch Desktop App**
```bash
pnpm -C client/desktop/tauri-app tauri dev
```

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
