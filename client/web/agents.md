cat > client/web/agents.md <<'EOF'
# QuanuX Web Agents (v0.0.1)

## Mission
This target is **presentation-only**. All computation happens on the backend. Agents must:
- **NOT** implement business logic or heavy compute in the frontend.
- Fetch data via backend APIs/bridge and render only.

## Stack
- Vite + React + TypeScript
- Tailwind + shadcn/ui (Radix)
- Import alias: `@/*` → `src/*`, `@/shared/*` → `../shared/*`

## Data Flow
- Use `@/shared/lib/bridge` for calling the bridge/backend (REST/WebSocket as provided).
- Do not add hooks that compute or cache domain logic on the client.

## Design + Figma MCP
- `client/integrations/figma` exposes MCP for design artifacts across all targets.
- Web uses those artifacts for tokens and UI composition only.

## Allowed UI
- Components from shadcn/ui and local wrappers.
- Minimal client state; never derive analytics, indicators, or trading logic here.

## ENV
- Refer to `README.md` and `.env.example` for `VITE_*` settings.

## Testing
- Component unit tests only. No integration tests that rely on compute.
EOF
