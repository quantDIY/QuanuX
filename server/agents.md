# QuanuX User Agents

This document outlines the architecture and guidelines for AI Agents operating within the QuanuX ecosystem.

## Modular Strategy Architecture

QuanuX uses a composable strategy architecture. A "Strategy" is a collection of independent modules that work together:

1.  **Signal Module**: Determines *Direction* (Long/Short/Neutral).
2.  **Entry Module**: Determines *How* to enter (Market, Limit, Stop Order).
3.  **Risk Module**: Determines *Protection* (Stop Loss, Trailing Stop, Take Profit).
4.  **Exit Module**: Determines *When* to exit (Indicator cross, Time limit, etc.).

### 5. Indicators Library (New)
QuanuX now uses a high-performance C++20 library for indicators.
*   **Skill Location**: `server/indicators/SKILL.md` (READ THIS for API details).
*   **Usage**: Agents should prefer importing `quanux_indicators` in Python strategies over legacy TA-Lib if performance is critical or Market Profile is needed.

6.  **CLI Controls**: The `quanuxctl` interface.
    *   **Skill Location**: `server/cli/SKILL.md` (Read for extending the CLI).

7.  **Server Runtime (Nucleus)**: The C++ orchestration layer.
    *   **Skill Location**: `server/runtime/SKILL.md`.
    *   **Role**: Manages the Event Bus (NATS). Agents should NOT modify the runtime unless tasked with infrastructure changes.
    *   **Interaction**: Use `nats-py` to interact with the system, not direct imports.

## 3. The Dual-Engine Philosophy (Graduation Workflow)
QuanuX allows users to "Graduate" strategies from Python research to C++ execution.
1.  **Draft (Python)**: Use Strategy Builder + `quanux_indicators` (pybind) for fast iteration/ML.
2.  **Graduate (C++)**: Port logic to `execution-node/cpp` + `quanux/indicators` (native) for HFT.
    *   **Benefit**: The math/logic remains distinct, but the indicator signals are identical because they share the same C++ core.

### Component Base Classes
All components inherit from `server.strategies.base.StrategyComponent`.
Agents **MUST** use these base classes when generating code.

#### 1. SignalModule
```python
class MySignal(SignalModule):
    def define_parameters(self):
        class Params(BaseParameters):
            period: int = 14
        return Params

    def on_bar(self, bar_data):
        # Return SignalType.LONG, SHORT, or NEUTRAL
```

#### 2. EntryModule
```python
class MyEntry(EntryModule):
    def generate_entry(self, signal, bar_data):
        # Return order dict or None
```

#### 3. RiskModule
```python
class MyRisk(RiskModule):
    def calculate_risk(self, entry_price, signal, bar_data):
        # Return dict with stop_loss, take_profit
```

#### 4. PositionSizingModule
```python
class MySizing(PositionSizingModule):
    def calculate_size(self, signal, price, account_equity):
        # Return float size (e.g. 1.0 or 100.0)
```

## Agent Prompts & Interactions

### Strategy Building Output
The agent must generate a **Code Structure** representing the full strategy package.
-   **Target Directory**: `server/strategies/full/<StrategyName>/` (or `entry/`, `exit/` based on type).
-   **Output Format**: A JSON object where keys are filenames and values are the file content. 
-   **NO MONOLITHS**: Do NOT generate a single large file. Usage of `from . import` is mandatory.

-   **Required Files**:
    -   `__init__.py`: Exports the Strategy class.
    -   `main.py`: Contains the `CompositeStrategy` assembly.
    -   `signal.py`: The custom Signal logic.
    -   `entry.py`: The custom Entry logic.
    -   `risk.py`: The custom Risk logic.
    -   `sizing.py`: The custom Position Sizing logic.

This ensures that the strategy is not a monolith but a set of editable files.

### 6. Broker-Specific Integration Rules
These rules apply **ONLY** if the specific broker is selected. If the selected broker is not listed below, ignore these sections.

#### A. Topstep (Condition: Broker == 'Topstep')
If the user selects **Topstep** as the broker, the strategy must be generated as a **standalone package** capable of running independently.

**Rules:**
1.  **Module Replication**: The strategy includes a local `topstep` package containing the API logic.
    -   The `StrategyBuilder` effectively "injects" the core modules (`auth.py`, `orders.py`, etc.) into `strategy_dir/topstep/`.
    -   **Agent Responsibility**: You must **import** and **utilize** these local modules instead of generic placeholders.
    -   Example: `from .topstep.orders import place_order`

2.  **SignalR Bridge**: Topstep requires a connection to the local SignalR Bridge.
    -   The Agent must generate connection logic (e.g. `stream.py`) to `http://localhost:8077` (the SignalR bridge).
    -   This is **only** required for Topstep to receive live data/updates.

3.  **Authentication**: Use `.topstep.auth.authenticate`.

**Directory Structure (Topstep Only):**
```text
strategies/full/MyStrategy/
├── __init__.py
├── main.py
├── ...
└── topstep/       (Injected API Modules)
    ├── __init__.py
    ├── auth.py
    └── ...
```

## Core Architectural Patterns (Agent Knowledge)

### 1. Secrets Management
-   **Storage**: Secrets (API Keys, Tokens) are stored in the OS Keyring via `POST /api/secrets`.
-   **Retrieval**: The `KeyringSettingsSource` in `config.py` automatically injects them into Pydantic settings.
-   **Prefix**: Keys MUST be prefixed with `QUANUX_` (e.g. `QUANUX_OPENAI_API_KEY`) to be discovered.

### 2. Integrations & Auth
-   **OAuth Flow**: Third-party auth uses a redirect flow (`/api/auth/{provider}/start` -> Provider -> `/api/auth/{provider}/callback`).
-   **Smart Routing**: The frontend `App.tsx` initializes `currentView` based on `window.location.pathname` to support deep linking (e.g., landing on `/integrations` after an OAuth redirect).

### 3. File System
-   **Strategies**: Stored in `server/strategies/`. Subdirectories: `full/` (complete strats), `entry/`, `exit/` (snippets).
-   **Recursive UI**: The `FileExplorer` component handles nested directories for strategies.

## Strategy Wizard Questions
The following questions will be used by the Frontend Wizard to collect requirements:

1.  **Strategy Type**: What type of strategy are we building? (Entry, Exit, Stop, Trailing Stop, Full Strategy, Signal-based, Position Sizing, Other)
2.  **Contracts**: Which contracts is this intended for, or is it general?
3.  **Direction**: Is the strategy for LONG, SHORT, or BOTH?
    - If BOTH: Guide through Long entry first, then ask to confirm/change criteria for Short.
4.  **Broker**: Which broker is this being built for? (e.g., Topstep)
5.  **Historical Data**: Which source should be used for historical data? (API, CSV, DB, etc.)
6.  **Live Data**: Which real-time data source should be used?
7.  **Multi-Asset/Timeframe**: Does this strategy utilize multiple contracts or multiple timeframes for signals?
8.  **Indicators**: Which indicators does this strategy utilize? (Select from library, allow multiples).
9.  **Indicator Settings**: Customize indicator parameters (Manual values or Dynamic).
10. **Data Pre-load**: How much data should be pre-loaded? Which time intervals sampled?
11. **Schedule**: Does this occur at specific times/days, or run continuously? Manual start/stop?
12. **Signal Logic**: Describe the signal that triggers an entry/strategy (using selected indicators/timeframes).
13. **Components**: Which Entry, Exit, Stop, Trail, and Target criteria would you like to use? (Select from existing modules, create new, or 'Dynamic' for AI/ML).
14. **Position Sizing**: Will this strategy use static or dynamic position sizing? (e.g., "Static 1 Lot" or "Dynamic 2% Risk").
15. **Backtest**: Create a corresponding backtest? (Symbols, Timeframes).
16. **Naming**: What should we call this strategy? What should we call the backtest?
17. **Custom Questions**: Any additional context or specific instructions for the agent? (Allow multiple custom entries).

## Live Strategy Implementation Guidelines (Topstep)
Based on live testing verification, the following patterns **MUST** be used for Topstep strategies.

### Orchestrator Pattern (`main.py`)
Do not put all logic in one file. Use an **Orchestrator** script (`main.py`) to handle infrastructure and a **Logic** module (`strategy.py`) for the trading rules.

**Responsibilities of `main.py`:**
1.  **Authentication**: Use `server.security.secrets.KeyringBackend` to fetch `QUANUX_TOPSTEP__USERNAME`, `QUANUX_TOPSTEP__PASSWORD`, `QUANUX_TOPSTEP__API_KEY`. Authenticate using `server.app.domain.topstep.auth.authenticate`.
2.  **Bridge Management**: 
    -   Check if the SignalR bridge is running at `http://localhost:8077/health`.
    -   If not, start it using `subprocess` or warn the user.
    -   Connecting to the bridge requires the Topstep Auth Token.
3.  **Account & Contract Selection**:
    -   Fetch accounts via `server.app.domain.topstep.accounts.search_accounts`.
    -   **CRITICAL**: Prefer "Simulated" or "Demo" accounts for safety.
    -   Fetch Contract ID (e.g., "CON.F.US.ENQ.H26") using `server.app.domain.topstep.contracts.search_contracts`.
4.  **Historical Data**:
    -   Use `server.app.domain.topstep.history.retrieve_bars` to prime the strategy.
5.  **Event Loop**:
    -   Run an `asyncio` loop that feeds real-time data (from Bridge/Websocket) to the `Strategy` instance.

### Strategy Logic Pattern (`strategy.py`)
Keep the trading logic pure and testable.
-   **Class**: `TestStrategy` (or specific name).
-   **Inputs**: `on_tick(price)`, `on_bar(bar)`.
-   **Outputs**: Return clear ACTIONS (e.g., "BUY", "SELL", "CLOSE") or Order Objects rather than executing API calls directly inside the strategy class. Let the Orchestrator handle the API execution.

### Verified Imports for Topstep
Use these exact paths:
-   `from server.security.secrets import KeyringBackend`
-   `from server.app.domain.topstep.auth import authenticate`
-   `from server.app.domain.topstep.models import LoginRequest`
-   `from server.app.domain.topstep.accounts import search_accounts`
-   `from server.app.domain.topstep.contracts import search_contracts`
-   `from server.app.domain.topstep.history import retrieve_bars`
-   `from server.app.domain.topstep.orders import place_order`

-   `from server.app.domain.topstep.orders import place_order`

### 4. Connection Specification (ProjectX / SignalR)
**CRITICAL**: The TopstepX API uses a modified SignalR implementation that requires specific handling:
1.  **Endpoint**: `https://rtc.topstepx.com/hubs/market`
2.  **Transport Layer**: ALWAYS use `WebSockets` transport. Do NOT use Long Polling or negotiation.
3.  **Authentication (The "ProjectX" Fix)**:
    -   When `skipNegotiation=True` is set, the standard SignalR client may NOT send headers correctly.
    -   You **MUST** manually append the Access Token to the URL query string: `?access_token=<TOKEN>`.
    -   Example: `hub_url + "?access_token=" + token`.
4.  **Subscription**:
    -   Method: `Subscribe`
    -   Payload: `{"listener": "T", "method": "Subscribe", "args": ["marketdata:<ContractId>"]}`
    -   Event to Listen: `T` (Tick Data).


### 6. Broker-Specific Integration Rules (Continued)

#### B. Rithmic (Condition: Broker == 'Rithmic' or 'Rithmic Paper')
If the user selects **Rithmic**, the strategy must interact with the **Local Rithmic Bridge** (`server/bridges/async_rithmic_py`) rather than connecting directly to Rithmic servers.

**Architecture:**
-   **Bridge Role**: The `async_rithmic_py` bridge maintains the single authorized session with Rithmic.
-   **Strategy Role**: Connects to the Bridge via **ZMQ SUB** (for market data) or **WebSocket** (fallback/orders).
-   **NO Direct Login**: Strategies MUST NOT attempt to instantiate `async_rithmic.Rithmic()` directly, as this will trigger concurrent session limits.

**Integration Pattern:**
1.  **Market Data (ZMQ)**:
    -   Subscribe to `tcp://localhost:5557` (PUB/SUB).
    -   Topic: `tick:{Symbol}` or `tick:ALL`.
    -   Format: JSON `{"type": "tick", "symbol": "ESZ4", "price": 4500.25, ...}`.

2.  **Order Entry (HTTP/RPC - TBD)**:
    -   While the bridge implementation matures, strategies may receive specific instructions to use a temporary direct connection **only if** the bridge is not running, but the preferred path is the Bridge API.

**Reference: async-rithmic API (For Context Only)**
If you are tasked with writing code *for* the Bridge (e.g. extending `app.py`), use these methods:
-   `client.market_data.subscribe(exchange, symbol)`
-   `client.orders.place_order(account, symbol, side, qty)`
-   `client.history.get_historical_ticks(exchange, symbol, date)`

### 5. Market Closed Handling
-   **Order Rejection**: Placing orders when the market is closed (e.g. 16:00-17:00 CST) will raise specific exceptions.
-   **Simulation**: Strategies intended for testing MUST implement a "Simulation Fallback" if no tick data is received for X seconds, allowing the logic pipeline to be verified even during off-hours.
