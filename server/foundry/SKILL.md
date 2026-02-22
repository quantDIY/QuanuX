---
name: QuanuX Foundry AI Core
description: Complete specification and operating procedures for the QuanuX Foundry AI code generation engine. Agent instructions for deterministic strategy compilation.
---

# QuanuX Foundry (Agent Operations)

> [!IMPORTANT]
> **IMMORTAL GUIDANCE**: You are an Agent operating within the QuanuX Foundry. Your primary directive is to generate **MATHEMATICALLY VERIFIABLE, DETERMINISTIC** trading components. You do NOT write monolithic Python scripts. You generate strictly typed payloads that survive cross-language execution.

The Foundry is the heart of QuanuX's generative AI capabilities. It answers the question: *How do we let an LLM write a trading strategy in Python, and instantly, safely deploy it to a C++ High-Frequency Trading execution node?*

## 1. Architectural Philosophy: The Sovereign Component

In the Foundry, code is not written; it is **Forged**.
Every Entity (Indicator, Entry Signal, Risk Model, Exit Strategy) is a **Sovereign Component**.
- It carries no state dependencies from a global server.
- It operates on a strict, mathematical Abstract Syntax Tree (AST).
- It is language-agnostic until the final compilation step.

## 2. The Generation Pipeline (IR -> AST -> Code)

When a user requests a new strategy component, you MUST follow this strict 3-step pipeline:

### Step 1: The Intermediate Representation (IR)
You **NEVER** write Python or C++ code immediately. You must first output a JSON object conforming to the Foundry IR Schemas (`server/foundry/schemas/`). 
The IR acts as the immutable middle-layer. It defines the inputs, the output shapes, and the logic gates mathematically.

*Schema Locations:*
- `indicator_ir.json`: Mathematical transforms over a price/volume array.
- `entry_ir.json`: Boolean gating logic determining *when* to enter.
- `strategy_ir.json`: The composition of indicators, entries, exits, and position sizing.

### Step 2: Language-Specific Translation (`FoundryGenerator`)
Once the IR JSON is created, the backend `FoundryGenerator` (`server/foundry/engine/generator.py`) takes that JSON and prompts an LLM (via `AgentRouter`) to translate the deterministic math into:
- **Python 3.14**: using GIL-free `asyncio.TaskGroup` architectures.
- **Cython 3.0**: using strict `cdef` memory layouts.
- **C++ 20**: using zero-allocation `std::ranges` and `quanux_indicators` headers.

### Step 3: The Deterministic Sandbox (`DeterministicSandbox`)
Before the component is saved to the disk, the Sandbox (`server/foundry/engine/sandbox.py`) executes the C++ variant, the Python variant, and the Cython variant simultaneously against identical mock Ohlcv bars.
If the C++ evaluates to `1.045` and the Python evaluates to `1.044`, the generation **FAILs** and must be re-prompted. Deterministic equivalence is the ultimate law.

## 3. Directory Taxonomy

When instructed to save or vendor a forged component, strictly adhere to this file structure inside `/server/foundry/`:

*   `/entries/<lang>/<version>/<component-name>/`
*   `/exits/<lang>/<version>/<component-name>/`
*   `/indicators/<lang>/<version>/<component-name>/`
*   `/risk/<lang>/<version>/<component-name>/`
*   `/strategies/<lang>/<version>/<strategy-name>/` (The composed package)

## 4. Submitting a Generation Job via Backend

The Foundry is heavily integrated with the FastApi, GraphQL, and NATS ecosystem.
Agents should trigger generation by hitting the REST endpoint, which subsequently queues the job on the NATS messaging bus.

**Endpoint:** `POST /api/foundry/forge`
**Payload:**
```json
{
  "component_type": "indicator",
  "name": "OrderBookImbalance",
  "target_lang": "cpp",
  "target_version": "20",
  "prompt": "Create an OBI indicator measuring the delta between top 5 bids/asks."
}
```

**Lifecycle Tracking:**
Upon submission, the API returns a `job_id` (e.g., `job_a1b2c3d4`). 
The client (React UI) will listen to the GraphQL Subscription `foundry_stream(job_id)` which is hooked to the NATS subject `sys.foundry.stream.<job_id>`. 
As the LLM generates tokens, those tokens stream down this pipe into the UI.

## 5. Testing & Verification

Agents must ensure the testing of components is handled by `POST /api/foundry/verify`.
**Payload:**
```json
{
  "strategy_name": "MyStrategy"
}
```
This forces the NATS bus to spin up the Deterministic Sandbox and assert the generated language files yield identical outputs. If they do not, the Sandbox publishes a divergence failure to the event bus.

## Agent Directives summary
1. ALWAYS use the `_ir.json` schemas as the truth source.
2. ALWAYS generate C++20 for high performance components.
3. NEVER assume UI elements contain business logic (all UI is "dumb" and uses GraphQL).
4. NEVER write single monolithic files; compose sovereign modules.
