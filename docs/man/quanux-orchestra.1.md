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

Initiates the bootstrap protocol. Operates as a pure, lobotomized fetcher to pull the raw 7.6MB `FIX.latest.xml` true instance from the FIX Trading Community without invoking any intermediate Java logic or parsers. This operates under a strict `sys.exit(1)` fail-fast network enforcement.

### compile
`quanuxctl orchestra compile --venue <venue_name>`

Triggers the C++ Standardizer API. Driven by `pugixml`, this native execution pipeline bypasses all JVM overhead:
1. Natively parses the true global standard XML, extracting 6,100+ strongly typed elements and codeSets.
2. Outputs Cython polyglot wrappers (`orchestra_constants.pxd` and `.pyx`) for exact memory parity between Python Nests and C++ logic.
3. Generates the `constants.hpp` header containing the entire FIX telemetry block and unmapped schema fallback integer (`99999`), and creates `venue_bridge.hpp` for the Tier-2 Spreader execution engines containing inline `constexpr` switch statements that physically map raw venue integers/chars to our globally unified enumerations.
4. Generates the `figi_mapper.hpp` layer, baking a strictly sorted 12-character FIGI translation array evaluated via a C++17 pure `constexpr std::string_view` binary search loop for nanosecond ticker translation.

**Example:**
```bash
quanuxctl orchestra compile --venue ibkr_onixs
```

### verify
`quanuxctl orchestra verify`

Injects a diagnostic checksum validation loop. It verifies the true, OS-level SHA-256 hash extracted via native `popen` against the master XML file. It ensures the physical hash baked into the C++ headers perfectly matches the Cython bindings across process boundaries.

## COMPLIANCE
All generated models guarantee the injection of `QuanuxUnmappedTag = 99999` to intercept schema drift events strictly off the execution hot path.

## AUTHOR
The Architect & Dr. Antigravity
