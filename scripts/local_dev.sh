#!/usr/bin/env bash
set -euo pipefail

echo "QuanuX local dev helper (scaffold) — v0.0.1"
echo "Checking tools…"

check() {
  if command -v "$1" >/dev/null 2>&1; then
    echo "  ✓ $1"
  else
    echo "  ✗ $1 (missing)"
  fi
}

check python3
check conda
check node
check pnpm || true
check rustc || true
check docker || true
check redis-server || true

echo
echo "Server (stub): would start Python backend here once wired."
echo "Client (stub): would run 'pnpm dev' in ./client once wired."
echo
echo "Tips:"
echo "  - conda activate quanux"
echo "  - corepack enable && corepack prepare pnpm@latest --activate"
echo "  - pnpm --filter quanux-client dev"
