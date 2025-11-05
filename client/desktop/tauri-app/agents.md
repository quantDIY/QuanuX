cat > client/desktop/tauri-app/agents.md <<'EOF'
# QuanuX Desktop (Tauri) Agents (v0.0.1)

## Mission
Desktop shell with React UI (same rule: **presentation-only**). Backend does all computation.

## Stack
- Tauri v2
- React + Vite (+ Tailwind, shadcn/ui)
- System tray support (Ubuntu GNOME), notifications, deep-links.

## Multi-instance
- Multiple windows and multiple processes are supported in future.
- Current dev scripts allow running multiple instances on different ports.
- Avoid per-window compute; use backend calls only.

## Data Flow
- Use `@/shared/lib/bridge` to talk to the backend.
- Tauri commands are for integration plumbing (window, tray, notifications), not compute.

## Figma MCP
- Same `client/integrations/figma` shared integration; desktop may pull tokens and assets.

## ENV
- `src-tauri/tauri.conf.json` handles app metadata and dev URL.
- UI respects the same env schema as Web for bridge endpoints.

## UX
- Tabs/multi-window patterns OK, but they must not compute locally.
- Respect OS integrations (tray menu, notifications) through Tauri APIs.
EOF
