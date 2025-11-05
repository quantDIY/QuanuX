cat > client/integrations/figma/agents.md <<'EOF'
# Figma MCP Integration (v0.0.1)

## Purpose
Shared design integration for all targets (web, desktop, mobile, TV).

## Guidelines
- Provide tokens, components metadata, and assets to front-ends.
- Do not push compute logic to clients; produce **design outputs** only.
- Keep API contracts stable; version schemas under `meta/schemas/json`.

## Future
- Theming pipeline per target
- Asset pipelines (icons, banners, TV screens)
EOF
