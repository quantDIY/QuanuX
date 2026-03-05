---
description: QuanuX Infrastructure - Terraform Genesis and VPC Matrix Security Rules
---

# QuanuX Infrastructure: Terraform Genesis & The Paranoia Firewall

## 1. Zero-Disk Keyring Doctrine
Terraform state and variables must NEVER store plain-text secrets on disk. The DigitalOcean API token and local SSH keys are dynamically fetched from the OS Keyring and injected into the Terraform variable space via `quanuxctl infra auth-shell`.

### Aleph Protocol Prerequisites
Before executing the Ansible playbooks, the following credentials MUST be explicitly generated and loaded into the local Keyring logic, bridging them directly into the execution shell environment:
- `MINIO_ROOT_USER`
- `MINIO_ROOT_PASSWORD`
- `HASURA_ADMIN_SECRET`
- `OPENSEARCH_ADMIN_PASSWORD`

## 2. The VPC Matrix (10.10.10.0/24)
All QuanuX nodes (Panopticon, Edge Sovereign Engines) are deployed into a strictly isolated, VPC-backed private subnet. 
- **Immutable Rule**: No internal node cluster communication takes place over the public internet.
- **Immutable Rule**: All CNATS JetStream mesh routing happens exclusively on the `10.10.10.x` IPs.

## 3. The Paranoia Firewall
The `quanux-paranoia-firewall` resource wraps the entire QuanuX cluster.
- **Inbound UDP/TCP**: Completely DROP all traffic that does not originate from within the `10.10.10.0/24` subnet.
- **SSH Exception**: Port 22 is only exposed to the single, dynamically injected `admin_ip` authorized during Terraform genesis.
- **Observability Bypass**: Any external CNATS metrics extraction from the Panopticon requires an encrypted SSH tunnel port-forward (`-L 4223:10.10.10.8:4222`) to pierce the IPv4 boundary.

## 4. Immutable Silicon Regeneration
Servers are cattle, not pets. We do not debug the Habitat soil in place if it is fundamentally contaminated. We rely on the absolute idempotency of our playbooks.
- **Action**: Use `terraform taint digitalocean_droplet.<node_id>` and cleanly redeploy it if environmental configuration drifts.
