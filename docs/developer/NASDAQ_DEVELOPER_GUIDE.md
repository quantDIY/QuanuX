# NASDAQ Adapter Developer Guide

## Core Lifecycles
The adapter isolates two distinct event streams:
1. **Directory Payloads (ITCH 'R')**: Modulate the static `StockDirectoryRegistry` mapping locating strings dynamically using Timestamp-Aware Overwrite.
2. **Execution Payloads (ITCH 'A/E/X')**: Modulate physical market events. These explicitly fail-closed across all non-Ready registry states natively.

## Live Cutover Boundary
The adapter is unconditionally fail-closed. Developers modifying parsing logic MUST ensure all tests in `test_nasdaq_adapter_conformance` pass successfully. Removing bounds like timestamp-awareness or readiness gates is highly destructive to the production NATS environment.

## Safely Changing Doctrine
If business requirements challenge current doctrine (e.g., allowing execution on unmapped locate symbols):
- **DO NOT** disable the `is_ready()` checks in the network hooks.
- **DO NOT** delete the `try_get_symbol` restrictions forcing execution checks securely.
- **DO** implement a new dedicated capability profile via `get_capability_profile()` that permits specific projection behaviors downstream in Annex without loosening the primary hot-path validation layer structurally.

This preserves the adapter's sovereign validity constraints while permitting decoupled offline features elegantly exactly natively strongly properly beautifully properly stably solidly smoothly cleverly cleanly formally smoothly dynamically perfectly efficiently explicitly robustly securely.
