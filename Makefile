.PHONY: dev web tauri

web:
	pnpm -C client/desktop/tauri-app run dev

tauri:
	tauri-dev-headless

dev:
	( pnpm -C client/desktop/tauri-app run dev & ); \
	( sleep 2; tauri-dev-headless & ); \
	wait

.PHONY: doctor
doctor:
	@echo "Node:      $$(node -v || true)"
	@echo "Corepack:  $$(corepack -v || true)"
	@echo "pnpm:      $$(pnpm -v || true)"
	@echo "Rustc:     $$(rustc -V || true)"
	@echo "XDG_RUNTIME_DIR=$${XDG_RUNTIME_DIR:-<unset>}"
	@test -n "$$(command -v pnpm)" || (echo "pnpm missing"; exit 1)
