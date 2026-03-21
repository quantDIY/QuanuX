# Tranche 4A Post-Implementation Risk Review

## 1. Local Cache Overwrite Risk
The JWKS cache is persisted to disk at `/tmp/qxctl_jwks_cache.json` with `0600` permissions. If an attacker operating as the local execution user modifies this file, they can inject a forged RSA public key. By forging the corresponding `kid` in a local JWT, the CLI would successfully mathematically authenticate the tampered token against the locally tampered trust root.

## 2. Hardcoded JWKS Network Failures
The outbound HTTP client mapping `QX_VAULT_JWKS_URL` does not enforce strict TLS certificate pinning. If intercepted locally, an attacker could route `QX_VAULT_JWKS_URL` to a malicious JWKS endpoint, successfully bypassing physical Vault network trust without triggering TLS enforcement errors.

## 3. Token Storing Vulnerabilities
Because the upstream Vault OIDC browser-flow (`vault login -method=oidc`) remains mocked, operators must manually supply the `QX_VAULT_TOKEN` environment variable to authenticate the CLI. This potentially leaks valid JWTs into persistent `~/.bash_history` files breaking token confidentiality globally.
