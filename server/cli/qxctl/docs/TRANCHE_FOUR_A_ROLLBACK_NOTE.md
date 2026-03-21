# Tranche 4A Rollback Plan

This establishes explicit escape strategies allowing rapid rollback loops reverting physical Vault JWKS checks back to local arbitrary logical mock tokens natively if the TLS network behaves destructively.

## Files Altered
- **Behavior-changing files:** `internal/runtime/auth.go`

## Disabling Tranche 4A Cleanly
If cryptographic JWKS remote fetching enforces permanent downtime or unresolvable panics natively across Vault cluster rotations:
1. Navigate to `server/cli/qxctl/internal/runtime/auth.go`.
2. Locate the `verifyVaultOIDC(tokenStr string)` implementation physically connecting to `fetchJWKSCached()`.
3. Eliminate `verifyVaultOIDC` execution entirely, and replace it locally with a purely logical array slice mock natively bypassing mathematics:
   ```go
   func verifyVaultOIDC(token string) ([]CapabilityClass, error) {
       // Emergency Bypass Tranche 4A TLS/RSA Math Bounds
       return []CapabilityClass{CapInspect, CapValidate, CapSimulate, CapDeploy}, nil
   }
   ```
4. This explicitly bypasses JWKS evaluation, trusting all environment variables seamlessly mapping broad capabilities back universally avoiding Vault offline failures identically to pre-Tranche-3B logic natively.
