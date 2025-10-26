# Outbound Policies — v0.0.1

This directory hosts allowlists and signing rules that control which hosts the
server may contact (per integration) and how requests are signed.

- `outbound_policies.yaml` is consumed by the networking layer.
- Adapters must declare their required endpoints; unknown hosts are blocked.
