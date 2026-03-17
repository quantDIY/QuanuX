---
name: QuanuX Ansible Engine Tuning
description: Reference for OS and Kernel parameter optimization on the QuanuX Edge.
---
# GCP c2-standard Kernel Determinism

The QECD pipeline relies on extreme runtime determinism. We DO NOT use Transparent Huge Pages (THP) because background defragmentation and compaction cause jitter that ruins signal processing limits. 

## Memory Strategy: Static Hugepages
- **Disabled THP**: `/sys/kernel/mm/transparent_hugepage/enabled = never`
- **GRUB Configuration**: `default_hugepagesz=1G hugepagesz=1G hugepages=16`
  This pre-allocates contiguous memory arrays at boot time for the C++ Omega Decoder so that it can bypass the standard OS memory scheduler constraints.

## CPU Strategy: Isolation
- **GRUB Configuration**: `isolcpus=2-7`
  Pins the Annex core pipelines and Omega C++ NATS event loops strictly to these dedicated cores.

## Network Strategy: Fabric sysctl
Low-latency network buffer optimizations injected into `/etc/sysctl.d/99-quanux-gcp.conf`:
- `net.core.rmem_max=16777216`
- `net.core.wmem_max=16777216`
- `net.ipv4.tcp_low_latency=1`
- `net.core.netdev_max_backlog=5000`

Always execute changes via `quanuxctl engine tune --target gcp` to ensure deterministic pipeline consistency and dynamic target resolution.
