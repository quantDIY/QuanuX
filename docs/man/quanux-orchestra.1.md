# QUANUX-ORCHESTRA(1)

## NAME
**quanux-orchestra** - Headless CLI for managing the QuanuX-Orchestra Universal Naming Registry and generating Tier-2 C++ Engine Bridges.

## SYNOPSIS
`quanuxctl orchestra <command> [options]`

## DESCRIPTION
The `quanuxctl orchestra` framework controls the Rosetta Stone of the QuanuX Matrix. It manages the strict compilation of global standards (ISO 20022, FIGI) into zero-latency C++ `constexpr` evaluation headers and Cython definitions.

## COMMANDS

### bootstrap
`quanuxctl orchestra bootstrap`

Initiates the bootstrap protocol. Operates as a pure, lobotomized fetcher to pull the raw `FIX.latest.xml` definitive schema from the FIX Trading Community without invoking any intermediate Java logic or parsers.

### compile
`quanuxctl orchestra compile --venue <venue_name>`

Triggers the C++ Standardizer API. Driven by `pugixml`, this native execution pipeline bypasses all JVM overhead:
1. Natively parses the raw venue-specific dictionary mapping (e.g., `venues/<venue_name>/broker.xml`) or universal standard XML.
2. Outputs Cython polyglot wrappers (`.pxd` and `.pyx`) for exact memory parity between Python Nests and C++ logic.
3. Generates the `venue_bridge.hpp` file for the Tier-2 Spreader execution engines. This file contains inline `constexpr` switch statements that physically map raw venue integers/chars to our globally unified `constants.hpp` enumerations with zero runtime overhead.

**Example:**
```bash
quanuxctl orchestra compile --venue ibkr_onixs
```

### verify
`quanuxctl orchestra verify`

Injects a diagnostic checksum validation loop. It verifies the embedded SHA-256 hash in the active Spreader engine binary matches the SHA-256 hash baked into the Python `superGraph` Cython bindings. Ensures absolute schema coherence across process boundaries.

## COMPLIANCE
All generated models guarantee the injection of `QuanuxUnmappedTag = 99999` to intercept schema drift events strictly off the execution hot path.

## AUTHOR
The Architect & Dr. Antigravity
