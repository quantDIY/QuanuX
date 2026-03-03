% QUANUXCTL-HABITAT(1) QuanuX Operations Manual | Version 1.0.0
% QuanuX Infrastructure Team
% March 2026

# NAME

**quanuxctl-habitat** - Prepare baseline OS infrastructure and bare-metal dependencies for QuanuX C++ execution nodes.

# SYNOPSIS

**quanuxctl habitat equip** *TARGET_GROUP*

# DESCRIPTION

`quanuxctl habitat equip` is responsible for conditioning virgin Droplets into operational QuanuX Edge Nodes. This command executes Stage 1 of the Two-Stage Immutable Deployment pattern.

It provisions all native OS packages, C++ dependencies (ZeroMQ, OpenSSL, DuckDB headers), IPC tuning, and dynamically writes the `.env` physics binding that the subsequent `Nest` deployment relies upon.

This command inherently targets `QuanuX-Infra/ansible/03a-habitat-equip.yml`.

## REQUIRED DEPENDENCIES (PROVISIONED BY HABITAT)

*   `g++-11`, `cmake`, `ninja-build` - C++20 Compilation Toolchains
*   `libssl-dev` - Native TLS/SSL acceleration
*   `libzmq3-dev` - Native ZeroMQ message multiplexing
*   `flatbuffers` - High-performance memory-mapped struct schema compilation
*   `duckdb.hpp` / `libduckdb.so` - In-memory analytical storage abstractions

# TARGET GROUPS

The *TARGET_GROUP* parameter corresponds directly to the dynamic inventory parsed by `ansible/dynamic_inventory.py`.

*   **edge_nodes**: Provisions the Tier 4 execution nodes situated physically closest to exchange matching engines.

# THE HABITAT.ENV CONTRACT

The critical output of `quanuxctl habitat equip` is the generation of `/etc/quanux/habitat.env`.

This file statically bounds the Edge Node to the `10.10.x.x` QuanuX Matrix VPC by leveraging Ansible dynamic inventory pipelines (`hostvars` templating) to extract the active Terraform Outputs and pin the `NATS_URL` directly to the internal interface of the `panopticon_buffer` instance. Legacy hardcoded IP assignments are structurally prohibited.

Example output synthesized dynamically on the execution server:
```
NATS_URL="nats://10.10.10.x:4222"
QUANUX_ENV=production
```

# ERRORS AND ABORTS

The Habitat deployment is designed to fail catastrophically if dependencies cannot be met. Do not intervene via SSH; diagnose the `quanuxctl` failure, adjust the Ansible `03a-habitat-equip.yml`, and scorch the Droplet via Terraform if necessary.

# SEE ALSO

`quanuxctl-nest(1)`, `quanux-infrastructure-skill(7)`
