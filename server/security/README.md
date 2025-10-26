# QuanuX Security (Scaffold) — v0.0.1

This folder contains *stubs* and guidance for auth, secrets, and transport security.
No production credentials or keys are included.

## Components
- `auth.py` — auth façade (WebAuthn/YubiKey/OAuth2 token intake; session and signing stubs)
- `secrets.py` — unified secrets broker (Keyring/Vault/Redis-cache adapters; no plaintext)
- `policies/` — outbound egress allowlist and signing/encryption policies
- `certs/` — mTLS placeholders (local dev only; never commit real keys)
- `providers/` — per-provider auth helpers (e.g., tradingview, broker SSO), scaffolds only

## Principles
- **Least privilege**: narrow scopes, short TTLs.
- **No plaintext** secrets at rest.
- **mTLS** for server↔server (and optionally client↔server).
- **Idempotency + Nonces** for all order/money-moving calls.
- **PFS preferred** ciphersuites when available.

## Wiring Order (later)
1) Implement `SecretsBroker` backends (keyring, vault, redis-cache).
2) Wire `AuthManager` flows (WebAuthn/YubiKey/OAuth2) into API layer.
3) Enable mTLS on internal hops; rotate keys via ops playbooks.
4) Enforce outbound policy allowlist per integration adapter.

See `/docs/Security/Secrets.md` and `/docs/Security/mTLS.md` for detailed policies.
