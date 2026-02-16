# Frontend Integration Audit & Migration Strategy

## Executive Summary
This document establishes the methodology for identifying, cataloging, and replacing "mock" or "placeholder" data in the QuanuX frontend with real, high-performance QuanuX protocols (GraphQL Subscription / NATS). It serves as the authoritative guide for the "Integration Audit" phase.

## 1. Audit Methodology
We will perform a systematic scan of the `client/` directory to identify all files containing:
-   Hardcoded JSON data arrays.
-   Hooks returning static data (e.g., `useMockData`).
-   Comments explicitly labeled `// MOCK`, `// TODO`, or `// PLACEHOLDER`.
-   Components with no data fetching logic (pure UI shells).

### Scanning Strategy (The "Mock Hunter")
We will use `grep` and manual code review to tag files with the following status levels:
1.  **🔴 PURE_MOCK**: Entire component logic is fake.
2.  **🟡 MIXED**: Real structure but hardcoded values/types.
3.  **🟢 WIRED**: Connected to `urql` (GraphQL) or `nats-ws`.

## 2. Integration Standard (The "QuanuX Protocol")
All frontend data fetching MUST adhere to the **Backend-Driven Client Architecture** defined in `GEMINI.md`.

### Rules of Engagement
1.  **No Client-Side Business Logic**: Clients are dumb terminals. They display what the server sends.
    -   *Forbidden*: `const total = price * quantity` in a `.tsx` file.
    -   *Required*: `total` comes pre-calculated from the GraphQL resolver.
2.  **Subscriptions Over Queries**: For market data, strategy states, and PnL, use **GraphQL Subscriptions** (via `urql` + `graphql-ws`).
3.  **Strict Typing**: All hooks must use auto-generated TypeScript types from the GraphQL schema.
4.  **Error Handling**: Every hook must handle `fetching`, `stale`, and `error` states explicitly.

## 3. Migration Workflow
For every identified "Mock Artifact":

1.  **Define Schema**: Create/Update the GraphQL schema in `server/graphql/schema.py` (or `.graphql` files) to support the required data.
2.  **Implement Resolver**: Wire the schema to the Python backend (FastAPI/Ariadne), ensuring it pulls from the NATS stream or Redis cache.
3.  **Generate Types**: Run `codegen` to produce the TypeScript types for the frontend.
4.  **Replace Hook**: Swap `useMockX` with `useSubscription` or `useQuery`.
5.  **Verify**: Confirm data flow from `Engine -> NATS -> Server -> Client`.

## 4. Scaffold Plan (The "Living Document")
We will maintain a `docs/technical/INTEGRATION_STATUS.md` file tracking the migration progress.

### Structure Template
| Component | Hook Name | Status | Backend Resolver | Priority |
| :--- | :--- | :--- | :--- | :--- |
| `QuoteMonitor` | `useQuotes` | 🔴 MOCK | `Subscription.marketData` | HIGH |
| `OrderTicket` | `useSubmitOrder` | 🔴 MOCK | `Mutation.submitOrder` | HIGH |
| `Accountheader`| `useAccountSummary`| 🟡 MIXED| `Query.accountSummary` | MED |

## 5. Immediate Action Plan
1.  **Scan**: Run the initial audit scan today.
2.  **Catalog**: Populate the `INTEGRATION_STATUS.md` table.
3.  **Scaffold**: Create "Empty" GraphQL resolvers that map to the identified needs, even if they return static data initially (moving the mock to the server is step 1).
