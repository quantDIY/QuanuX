# QuanuX Foundry

The **QuanuX Foundry** is an advanced AI orchestration engine designed to generate, verify, and deploy institutional-grade trading components. 

Rather than relying on humans to manually rewrite successful Python prototype strategies into performant C++ code for production execution, the Foundry utilizes Generative AI enforcing strict mathematical equivalence across language boundaries.

## The Problem
Quantitative researchers build models in Python because of its vast data-science ecosystem (Pandas, PyTorch). However, executing trades in microseconds requires C++. The translation process between a researcher's Python code and an engineer's C++ code is slow, error-prone, and prone to "translation drift" (where the C++ implementation slightly deviates mathematically from the backtested Python model).

## The Foundry Solution
The Foundry solves this by using AI to generate the code in all required languages simultaneously, governed by a strict JSON Intermediate Representation (IR), and proven correct via a Deterministic Sandbox.

### Key Components

1.  **Intermediate Representation (IR)**: (`/server/foundry/schemas/*.json`)
    The prompt is not "Write a MACD in C++." The prompt is structured into a mathematical AST (Abstract Syntax Tree) defined within JSON. The AI emits this JSON first.

2.  **Foundry Generator & Agent Router**: (`/server/foundry/engine/`)
    The engine reads the IR JSON, connects to the user's BYOK LLM (Gemini, Ollama), and strictly demands the generation of idiomatically correct code (e.g., Python 3.14 free-threading, C++20 Ranges) based *only* on the logic present in the IR.

3.  **Deterministic Sandbox**:
    Before you, the human, even see the generated code, the Sandbox compiles the C++ variant (via cython bindings) and the Python variant, feeds both of them an identical `[OHLCV]` array, and asserts that their state and outputs match exactly down to the floating-point decimal.

## API & GraphQL Integration

The Foundry is heavily decoupled and relies on the NATS messaging bus. 

*   `POST /api/foundry/forge` -> Publishes to NATS `sys.foundry.request.forge`
*   `POST /api/foundry/verify` -> Publishes to NATS `sys.foundry.request.verify`

Progress and real-time streaming of tokens to the React frontend are handled via GraphQL Subscriptions attached to the NATS `sys.foundry.stream.<job_id>` subject.

## Directory Structure
Generated artifacts are saved by language and version to prevent monoliths.
```text
/server/foundry/
  ├── entries/
  │   ├── python/3.14/MyEntry/
  │   └── cpp/20/MyEntry/
  ├── indicators/
  ├── schemas/ (The IR JSON definitions)
  └── engine/ (The API generation & sandbox code)
```
