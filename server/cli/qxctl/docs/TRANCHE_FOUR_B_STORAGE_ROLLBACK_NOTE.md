# Tranche 4B Option 2 (Storage) Rollback Plan

## Files Altered
- `cmd/storage.go`

## Disabling Tranche 4B Target Tracking
If the operational capability constraints mapped across explicit NVMe paths unexpectedly break S3 mappings or DuckDB local ingestion routes causing critical failures for verified active deployment operators:
1. Open `server/cli/qxctl/cmd/storage.go`.
2. Locate and delete the four `runtime.BindMetadata(...)` initialization strings linked structurally to `scanCmd`, `attachCmd`, `detachCmd`, and `initCmd`.
3. Erasing this structural enforcement array cleanly drops the entire `storage` and `duckdb` namespace natively back into the primary bypassed execution block. Vault cryptographic bounds processing telemetry safely continue working smoothly around the remaining covered set limits natively.
