cat > client/mobile/agents.md <<'EOF'
# QuanuX Mobile Agents (v0.0.1)

## Mission
React Native front-end. **Presentation-only**. Use backend for data/compute.

## Stack
- Expo + React Native
- Design tokens should align with Figma MCP outputs.

## Data Flow
- Call backend over HTTPS/WebSocket; never compute indicators/analytics on device.

## ENV
- Per-platform config (app.json) for bundle ids; use secure storage for tokens if needed.

## UX
- Use light data displays; avoid background work. Offline/queueing requires backend support.
EOF
