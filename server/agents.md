# QuanuX User Agents

This document outlines the architecture and guidelines for AI Agents operating within the QuanuX ecosystem. Specifically, it details how agents should construct Modular Trading Strategies.

## Modular Strategy Architecture

QuanuX uses a composable strategy architecture. A "Strategy" is a collection of independent modules that work together:

1.  **Signal Module**: Determines *Direction* (Long/Short/Neutral).
2.  **Entry Module**: Determines *How* to enter (Market, Limit, Stop Order).
3.  **Risk Module**: Determines *Protection* (Stop Loss, Trailing Stop, Take Profit).
4.  **Exit Module**: Determines *When* to exit (Indicator cross, Time limit, etc.).

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

## Agent Prompts & Interactions

### Strategy Building Wizard
When the user uses the Strategy Builder Wizard, the AI Agent will receive:
1.  **Context**: This `agents.md` file.
2.  **User Answers**: The data collected from the wizard questions.
    - *Example*: "I want an RSI Strategy on 15m timeframe that buys when RSI < 30 and sells when RSI > 70. Use a 20 tick stop loss."

### Agent Output Requirements
The agent must generate a **Code Structure** representing the full strategy package.
-   Target Directory: `server/strategies/full/<StrategyName>/` (or `entry/`, `exit/` based on type).
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
