---
description: QuanuX-Annex Federation Layer
---

# The Federation Layer Context

This directory represents the QuanuX-Annex boundary logic with the centralized Hasura GraphQL engine. It implements the C++ HTTP webhook listeners required by Hasura Remote Schemas.

## Core Architectural Rules
- **Non-Blocking Egress**: Do not block the `cpp-httplib` server thread pool. The system operates under extreme concurrency constraints.
- **cpp-httplib ThreadPool Expansion**: During Phase 14.5, we removed the strict 4-thread clamp on `m_server.new_task_queue = [] { return new httplib::ThreadPool(128); };`. This prevents single long-running DO DO Spaces fetch requests from deadlocking the HTTP server and starving subsequent Hasura queries.
- **JSON Pipeline Streaming**: The native `nlohmann::json` constructs massive DOM objects in RAM. When handling millions of `MarketTick` rows, this triggers instant OOM kills on edge instances. As cemented in Phase 14.5, DO NOT use `nlohmann::json` to build the full array. Instead, use `res.set_chunked_content_provider` to proxy raw chunked output via `snprintf` strings formatted dynamically from binary array indices.

## Historical & Agent Context
When iterating on the webhook server, always assume the input is deeply nested JSON originating from Hasura. It must answer quickly or use chunked encoding to satisfy the Reverse Proxy timeout limits. We successfully built a zero-allocation streaming integration linking the S3 Binary Blobs direct to standard Hasura JSON formats by bridging `libcurl` callbacks with `httplib` pipe functions.
