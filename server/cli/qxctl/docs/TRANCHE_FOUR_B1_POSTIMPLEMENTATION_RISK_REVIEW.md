# Tranche 4B1 Post-Implementation Risk Review

## 1. Sub-Tranche Scope Limits
The scope expansion in 4B1 applies `CapInspect` and `CapDeploy` limits strictly to the `telemetry` module. The primary risk for the next stage (4B2) is assigning `CapInspect` to `dashboard start` without runtime validation to confirm the command does not perform hidden writes or background mutation.

## 2. Incomplete Command Coverage
The uncovered-command bypass in `auth.go` remains intentionally active. While this preserves existing behavior for uncovered modules (e.g. `engine`, `nest`), those modules remain entirely unprotected by capability verification until explicitly mapped.
