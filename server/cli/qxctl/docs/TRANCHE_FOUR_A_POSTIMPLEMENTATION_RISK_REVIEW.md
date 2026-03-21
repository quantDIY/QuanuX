# Tranche 4A Post-Implementation Risk Review

This document audits the remaining physical leak paths explicitly existing natively after introducing physical Vault Cryptographic JWKS bounds natively.

## 1. Local Cache Integrity Bypasses
- **Unencrypted Persisted Caching:** The new cryptographic bounds securely cache the JSON Web Key Sets (JWKS) to disk (`/tmp/qxctl_jwks_cache.json`) natively to bridge short-lived CLI command executions reliably masking network constraints. This file is written using `0600` permissions. However, malware structurally operating as the executing user locally could trivially overwrite the cached `.json` injecting a malicious `n`/`e` block mapping a locally-forged RSA key if they accurately forge the `kid` primitive, tricking the CLI into effectively authenticating tampered local physical tokens blindly against a fake structural root of trust locally.

## 2. Hardcoded JWKS Network Failures
- **TLS Bypass Risks:** The outbound `http.Client` explicitly mapping the `QX_VAULT_JWKS_URL` environment variable does not constrain custom certificate pinning natively. While Vault implies TLS, if an attacker globally injects `QX_VAULT_JWKS_URL=http://localhost:8200/keys` directly intercepting the `go` network stack execution locally, the system physically respects it blindly bypassing physical network trust layers without structured TLS identity verification natively verifying the CA.

## 3. OIDC Login Extraction Mock
- **Backend-Only Trust Verification:** Because the frontend OAuth2 token negotiation flow natively opening up an interactive browser window generating the physical JWT locally is not implemented, operators structurally generate tokens independently. If operators copy-paste tokens across insecure bash histories globally generating `QX_VAULT_TOKEN=...`, these sensitive RS256 secrets natively leak across `~/.bash_history` files breaking token confidentiality globally.
