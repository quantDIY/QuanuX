# QuanuX Data Layer Architecture

This document defines the **Abstract Data Layer** for the QuanuX ecosystem. It allows clients (Frontend, Agents, Extensions) to consume data without tight coupling to the underlying implementation (FastAPI vs Flask, NATS vs Redis).

## 1. The Core Principle: Capability over Implementation

We define data access by **Capability**, not by technology. A "Market Data Service" is defined by *what it provides*, not *how it runs*.

## 2. Communication Patterns

All data exchange in QuanuX falls into one of two patterns. Clients MUST support both.

### Pattern A: Request/Response (Command & Query)
**Purpose**: User actions, configuration updates, and fetching static state.
-   **Abstraction**: "RPC" or "API Endpoint".
-   **Discovery**: `docs/openapi.json` or `meta/api/schema.yaml`.
-   **Transport**: Typically **HTTP/REST**, but could be gRPC or NATS Request/Reply.
-   **Rule**: If you need to *change* state or *get* a snapshot, look for an API definition.

### Pattern B: Event Stream (Real-Time)
**Purpose**: Market data ticks, system health, order updates.
-   **Abstraction**: "Topic" or "Subject".
-   **Discovery**: `meta/events/catalog.yaml`.
-   **Transport**: **NATS** (Internal) / **SignalR** (Frontend).
-   **Rule**: If you need *live* updates, subscribe to a Subject. DO NOT poll the API.

## 3. The "Backend Origin" Contract

Regardless of the transport, the Backend is the Single Source of Truth.

1.  **Data Shape**: Defined by Server Pydantic Models.
2.  **No Transformation**: Frontend treats data as Read-Only.
3.  **Atomic Updates**: Event payloads are complete; partial updates are discouraged for safety.

## 4. Specific Implementations (Current)

While the architecture is abstract, the current mapping is:

| Capability | Abstract Pattern | Implementation |
| :--- | :--- | :--- |
| **User/Auth** | Request/Response | **FastAPI** (HTTP) |
| **Market Data** | Event Stream | **NATS** (Core) -> **GraphQL Sub** (Mobile) / **SignalR** (Web) |
| **System State** | Request/Response | **FastAPI** (HTTP) |
| **MCP Tools** | RPC | **Stdio** / **SSE** |

## 5. The Real-Time Pipeline (NATS -> GraphQL)

This is the primary artery for the Mobile Ecosystem (`client/react-native/*`).

### The Flow
1.  **Source**: The **Runtime Engine** (C++/Python) publishes "live" events (e.g., `market.NQ.tick`) to a local or remote **NATS** cluster.
2.  **Gateway**: The **FastAPI Server** (`server/app`) connects to NATS via `server/app/services/nats.py`.
3.  **Bridge**: A **GraphQL Subscription** (`server/app/graphql/schema.py`) listens to specific NATS subjects.
4.  **Client**: The Mobile App connects via WebSocket (`ws://api/graphql`) and subscribes:
    ```graphql
    subscription { marketData(symbol: "NQ") { price, ts } }
    ```

### Why this Architecture?
*   **Decoupling**: The Engine doesn't know about GraphQL. The Mobile App doesn't know about NATS.
*   **Efficiency**: NATS handles the high-throughput fanout. GraphQL provides specific, typed shapes to low-bandwidth mobile devices.
*   **Remote Capable**: Execution Nodes can be anywhere (AWS, etc.), pushing to the NATS cluster. The API server just listens.

## 6. Guide for Agents & Extensions

When generating code or architecture:

1.  **Ask**: "Is this data static (Configuration) or dynamic (Market Data)?"
2.  **Map**:
    -   Static -> Look for an API Client / Hook.
    -   Dynamic -> Look for a Socket / Stream Listener.
3.  **Implement**: Use the standard QuanuX hooks (e.g. `useMarketData`, `useQuery`), avoiding direct `fetch()` calls where possible.
