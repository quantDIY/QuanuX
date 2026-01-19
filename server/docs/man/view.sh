#!/usr/bin/env bash
set -euo pipefail

# Usage: ./view.sh [page_name]
# Example: ./view.sh quanuxctl-ext
# Default: quanuxctl

PAGE="${1:-quanuxctl}"
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Use -M to treat the current directory as the manpath root
# This allows 'man' to automatically find files in man1/, man7/ etc.
man -M "$DIR" "$PAGE"
