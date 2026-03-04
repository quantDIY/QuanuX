# QUANUXCTL-OBS(1)

## NAME
**quanuxctl obs** - QuanuX Observability Plane remote administration interface.

## SYNOPSIS
**quanuxctl** **obs** [COMMAND] [OPTIONS] [ARGUMENTS...]

## DESCRIPTION
The **obs** module of the **quanuxctl** command-line interface provides stateless, air-gapped control over the QuanuX Data & Telemetry Plane. Acting entirely as a CNATS publisher, the utility communicates with the internal C++ Sovereign Engine and Python Shadow Nodes without instantiating direct data-layer (DuckDB, ValKey, OpenSearch) connections.

## COMMANDS

### `config-exchange`
Submits an asynchronous update to the global Exchange Registry, dictating when an exchange triggers its settlement epoch.

**Usage:**
`quanuxctl obs config-exchange <EXCHANGE_CODE> <CLOSE_TIME_SECONDS> [OFFSET_MINUTES]`

**Arguments:**
* `EXCHANGE_CODE`: Global venue identifier (e.g., LSE, TSE, NYSE).
* `CLOSE_TIME_SECONDS`: The integer seconds past local midnight triggering settlement.
* `OFFSET_MINUTES`: Active offset from UTC for the local venue timezone (Handles fractional offsets. Default: 0).

### `trigger-freeze`
Forces a manual, out-of-band DuckDB settlement pull by the active C++ Settlement Daemon. It causes the C++ Engine to generate and flush Parquet files immediately regardless of the scheduled epoch.

**Usage:**
`quanuxctl obs trigger-freeze <EXCHANGE_CODE>`

**Arguments:**
* `EXCHANGE_CODE`: Force the frozen settlement state across the specified venue.

### `cluster-health`
Monitors the CNATS JetStream mesh firehose actively and aggregates Edge Node heartbeats to print a current health table of the global execution plane.

**Usage:**
`quanuxctl obs cluster-health [OPTIONS]`

**Options:**
* `--duration INTEGER`: The observation window to listen to the firehose in seconds (Default: 2).

## EXAMPLES

Configure the London Stock Exchange (LSE) to trigger its settlement epoch at 16:30 (59400 seconds) local time with a 0 UTC offset:
```bash
$ quanuxctl obs config-exchange LSE 59400 0
```

Configure the Tokyo Stock Exchange (TSE) to trigger its close with a standard +9 Hours (540 minutes) offset:
```bash
$ quanuxctl obs config-exchange TSE 54000 540
```

Force an immediate Parquet flush for the Nasdaq following a circuit breaker anomaly:
```bash
$ quanuxctl obs trigger-freeze NASDAQ
```

Map the live, active edge nodes processing trades across the execution grid:
```bash
$ quanuxctl obs cluster-health --duration 5
```

## ARCHITECTURE
The CLI is strictly stateless. Operations are achieved by publishing JSON dictionaries onto explicit CNATS control paths (e.g. `quanux.control.registry.config`, `quanux.control.obs.freeze`). Execution relies entirely on daemon presence and JetStream message queueing. 

**THE ALEPH PROTOCOL:** Analytical queries, AI Model Context Protocol (MCP) data assimilation, and frontend client retrievals do not execute through the CLI. They query the unified **panopticon-nexus Hasura GraphQL** Supergraph. The Nexus node natively bridges the OpenSearch text logs with the DuckDB telemetry Hive partitions, physically severing the Read Path from the Write Path.

## SEE ALSO
`quanuxctl(1)`
