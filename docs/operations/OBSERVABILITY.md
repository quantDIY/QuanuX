# QuanuX Institutional Runbook: Observability Operations

This runbook outlines the required deployment topologies for the QuanuX Data & Telemetry Plane (Operation Panopticon Genesis). The architecture dynamically scales from local development up to globally distributed Tier 1 execution environments.

## 1. Small (The Local Quant)
Designed for local strategy generation, algorithm testing, and individual portfolio visualization.

* **Execution Plane**: 1 C++ Execution Node running natively on the host workstation.
* **Mesh**: 1 embedded NATS JetStream server.
* **Observability Pod**: A single, local `docker-compose.yml` stack orchestrating:
  * ValKey (Hot Buffer)
  * Prometheus & Alertmanager
  * OpenSearch (Single-node configuration)
  * DuckDB C++ Writer instance pushing local Parquet files.
* **Shadow Node**: Local Python daemon routing NATS traffic into the docker stack.

## 2. Regular (The Lean Fund)
Designed for multi-venue retail execution or low-latency proprietary trading from dedicated servers.

* **Execution Plane**: 1 C++ Execution Node deployed per region (e.g., one in AWS us-east-1, one in eu-west-2).
* **Mesh**: Distributed CNATS cluster bridging regions.
* **Observability Droplet**: A heavy, isolated DigitalOcean Droplet (or AWS EC2 instance) solely dedicated to telemetry ingestion. It runs:
  * The Python Cython Shadow Node.
  * Standalone ValKey server.
  * Prometheus server aggregating regional execution targets.
* **Vault**: Regional C++ `SettlementDaemons` pushing directly into shared S3 backend buckets (DuckDB `COPY TO` S3).

## 3. HA (The Tier 1 Institutional Matrix)
Designed for unyielding uptime, high-frequency execution across major global exchanges, and zero-data-loss resiliency.

* **Execution Plane**: Minimum 3x C++ Execution Nodes per global hub, utilizing Raft Consensus protocols over CNATS for deterministic state locking.
* **Mesh**: Dedicated 5-node highly available NATS JetStream cluster globally load-balanced.
* **The Hot Buffer Matrix**: 3-Node ValKey Sentinel Cluster ensuring instant failover for live UI tickers and depth modeling.
* **The Vitals Core**: Active/Active Dual Prometheus Servers operating with isolated persistence maps.
* **The Forensic Ledger**: 3-Node minimum OpenSearch Cluster composed of Dedicated Master Nodes and heavy SSD-backed Data Nodes.
* **The Cold Vault**: Globally distributed DuckDB C++ `SettlementDaemons` pulling data per-exchange strictly based on the Exchange Registry Epochs, generating synchronized Parquet files into Multi-Region S3 buckets with aggressive lifecycle aging.
