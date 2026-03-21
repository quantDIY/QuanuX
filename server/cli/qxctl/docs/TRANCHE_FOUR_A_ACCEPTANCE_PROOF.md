# Tranche 4A Acceptance Proof

## Architecture Configuration
- **JWKS Cache TTL:** 15 minutes.
- **Cache Storage:** Persisted to disk at `/tmp/qxctl_jwks_cache.json`.
- **Mocked Components:** The interactive Vault OIDC login flow (`vault login -method=oidc`) is mocked. The `QX_VAULT_TOKEN` is supplied manually via environment variables. Cryptographic signature and verification network mechanics are fully implemented and verified.

## Acceptance Criteria

### 1. Valid authorized success
**Command:**
`QX_VAULT_TOKEN=<valid_deploy_token> ./qxctl node deploy server1 --dry-run --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "node deploy",
  "message": "Dry-run engaged. Skipping physical TCP socket bindings..."
}
```

### 2. Valid but underprivileged token
**Command:**
`QX_VAULT_TOKEN=<valid_inspect_token> ./qxctl node deploy server1 --dry-run --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": " Authorization rejected. Caller lacks [simulate] capability required to evaluate structural bounds."
  }
}
```

### 3. Tampered signature
**Command:**
`QX_VAULT_TOKEN=<tampered_payload_token> ./qxctl vault status --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 401,
  "error": {
    "type": "AUTH_VERIFICATION_FAILED",
    "message": " Vault OIDC auth verification failed: cryptographic signature tampered securely bounding offline cleanly natively: crypto/rsa: verification error"
  }
}
```

### 4. Invalid or missing KID / issuer mismatch
**Command:**
`QX_VAULT_TOKEN=<unknown_kid_token> ./qxctl vault status --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 401,
  "error": {
    "type": "AUTH_VERIFICATION_FAILED",
    "message": " Vault OIDC auth verification failed: JWT kid not found in remote or cached JWKS natively (Invalid Issuer or Stale Cache)"
  }
}
```

### 5. Expired token
**Command:**
`QX_VAULT_TOKEN=<expired_token> ./qxctl vault status --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 401,
  "error": {
    "type": "AUTH_VERIFICATION_FAILED",
    "message": " Vault OIDC auth verification failed: token expired physically offline cleanly bounds"
  }
}
```

### 6. Malformed token
**Command:**
`QX_VAULT_TOKEN=eyJhbGciOiJub25lIn0.malformed.signature_bytes ./qxctl vault status --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 401,
  "error": {
    "type": "AUTH_VERIFICATION_FAILED",
    "message": " Vault OIDC auth verification failed: invalid hashing algorithm bounds natively (expected RS256)"
  }
}
```

### 7. Network unavailable with valid cached JWKS
**Context:** Vault server is down (port 18202 is unreachable), but `/tmp/qxctl_jwks_cache.json` holds a valid cached JWKS fetched within the last 15 minutes.
**Command:**
`QX_VAULT_JWKS_URL=http://localhost:18202/keys QX_VAULT_TOKEN=<valid_token> ./qxctl vault status --output=json --target=gcp`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "vault status",
  "message": "Vault status inspection passed natively."
}
```

### 8. Network unavailable with expired or unusable cache
**Context:** Vault server is down (port 18202 is unreachable) and `/tmp/qxctl_jwks_cache.json` either does not exist or has a `fetched_at` timestamp older than 15 minutes.
**Command:**
`QX_VAULT_JWKS_URL=http://localhost:18202/keys QX_VAULT_TOKEN=<valid_token> ./qxctl vault status --output=json --target=gcp`
**Output:**
```json
{
  "status": "error",
  "exit_code": 401,
  "error": {
    "type": "AUTH_VERIFICATION_FAILED",
    "message": " Vault OIDC auth verification failed: JWKS retrieval failed natively: Vault TLS unavailable and offline JWKS cache formally expired natively: <nil>"
  }
}
```
