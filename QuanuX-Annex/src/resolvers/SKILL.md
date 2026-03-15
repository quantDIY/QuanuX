---
description: QuanuX-Annex Storage Abstraction Layer (SAL)
---

# The Storage Abstraction Layer (SAL) Context

The QuanuX-Annex `resolvers` directory implements the core cloud physics engines that interface with object storage and high-speed data meshes.

## Core Architectural Rules
- **Zero Allocation**: Do not use `std::string` or build DOM structures (like large `nlohmann::json` arrays) for telemetry bursts. Use tightly-packed C-struct memory (`MarketTick`, `ExecutionLog`).
- **NATS Decay Hazard**: NATS `natsOptions_SetURL(opts, url)` expects a globally persistent C-string. Do NOT pass temporary `c_str()` pointers derived from local string allocations (e.g., parsing from an environment file) unless they are safely persisted to a class-wide `std::string` member.
- **DO Spaces AWS SigV4**: DigitalOcean strictly requires exact SigV4 cryptography. We built a native OpenSSL implementation (`AwsSigV4.hpp`) securely deriving creds directly from the `/etc/quanux/habitat.env` keyring file parsing logic. Make sure to use `UNSIGNED-PAYLOAD` for the `x-amz-content-sha256` header on `PUT`/`GET` calls. Bypassing the synchronous chunk SHA256 hashing during TLS transfers prevents severe ingestion bottlenecks and DO Spaces `400 InvalidArgument` drops.
- **Asynchronous Execution**: Ingestion to S3 uses `ZarrRamFlusher::network_worker`, operating a dedicated `curl_multi` background thread.

## Historical Context & Run-ins
- **The "Expect: 100-continue" Trap**: When writing Zarr chunks, `libcurl` injected HTTP 100-continue headers by default. S3-compatible endpoints heavily reject these in specific situations causing invisible failures. Always explicitly strip this using `curl_slist_append(chunk, "Expect:");`.
- **The NATS C-API Decay Bug**: NATS would silently resolve corrupt hostnames because a `std::string::c_str()` decayed out of scope inside `NatsSubscriber.cpp`. Persist connection strings inside the class lifecycle.
