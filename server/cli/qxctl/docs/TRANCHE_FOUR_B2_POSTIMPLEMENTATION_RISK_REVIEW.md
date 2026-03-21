# Tranche 4B2 Post-Implementation Risk Review

## 1. Sub-Tranche Scope Limits
The scope expansion in 4B2 applies `CapInspect` strictly to the `dashboard start` module. The primary operational risk for future coverage waves (e.g., executing against `engine` or `nest`) is applying observational `CapInspect` mappings globally without continuing the mandatory leaf-by-leaf capability audits performed here. 

## 2. Incomplete Command Coverage
The uncovered-command bypass in `auth.go` remains intentionally active. While this preserves existing behavior for unmapped modules (e.g., `engine`, `nest`), those modules remain entirely vulnerable and unprotected by cryptographic capability verification until they are formally audited and placed into the covered set.
