---
name: Node Conditioning
description: The most basic infrastructure step for the QuanuX Organism. Defines the 'Outer Russian Doll' conditioning required before any execution-node deployment is permitted.
---
# Node Conditioning

**Category:** Infrastructure

## Prerequisites
- Ubuntu 22.04 LTS

## The Law
> **No execution-node deployment is permitted until Step 1 (`node_init.sh`) is verified.**

## Phases
- **OS Scrub**: `apt update/upgrade`, `install build-essential`, `curl`, `git`
- **Nutrient Layer**: `conda install -c quanux-repo set-quanux-node`
- **RSA Handshake**: Generate 4096-bit RSA keypair
- **NATS Grid**: Generate sovereign NKey pair (Ed25519); configure `nats-server.conf` for Route Handshaking on port 6222; `ufw allow 4222, 6222`

## Success Signal
`QUANUX_NODE_READY`
