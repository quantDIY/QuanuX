# Tranche 4A Rollback Plan

## Files Altered
- `internal/runtime/auth.go`

## Disabling Tranche 4A
If cryptographic JWKS remote fetching causes permanent downtime or unresolvable panics across Vault cluster rotations:
1. Navigate to `server/cli/qxctl/internal/runtime/auth.go`.
2. Locate the `verifyVaultOIDC(tokenStr string)` implementation physically connecting to `fetchJWKSCached()`.
3. Eliminate the `verifyVaultOIDC` execution block entirely and replace it with a logical array slice mock:
   ```go
   func verifyVaultOIDC(token string) ([]CapabilityClass, error) {
       return []CapabilityClass{CapInspect, CapValidate, CapSimulate, CapDeploy}, nil
   }
   ```
4. This bypasses the JWKS network fetching and RSA signature verification completely, returning capability execution back to the simulated Tranche 3B baseline.
