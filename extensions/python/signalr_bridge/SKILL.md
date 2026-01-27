---
name: signalr-bridge
description: Instructions for operating and configuring the SignalR Bridge for TopstepX.
---

# SignalR Bridge Skill

The **SignalR Bridge** (`quanux-bridge-signalr`) is a specialized extension that maintains persistent WebSocket connections to TopstepX's SignalR Hubs. It bridges the gap between the proprietary SignalR protocol and QuanuX's internal messaging.

## Location
- **Path**: `extensions/python/signalr_bridge`
- **Config**: `extensions/python/signalr_bridge/extension.yaml`
- **Node Runtime**: `extensions/python/signalr_bridge/src/node` (Preferred)
- **Flask Runtime**: `extensions/python/signalr_bridge/src/flask` (Legacy/Fallback)

## Architecture
The bridge runs as a standalone process (supervised by QuanuX).
- **Upstream**: Connects to `rtc.topstepx.com` (User/Market Hubs).
- **Downstream**: Exposes a wrapper WebSocket (or HTTP callbacks) to QuanuX components.

## Configuration (TopstepX Specific)
This bridge is pre-configured for TopstepX logic but is technically generic.

**Key Environment Variables (Managed via `quanuxctl topstepx env`):**
- `QUANUX_SIGNALR_USER_HUB`: URL for the User Hub (Login, Orders, Fills). 
  - Default: `https://rtc.topstepx.com/hubs/user`
- `QUANUX_SIGNALR_MARKET_HUB`: URL for the Market Hub (Quotes, Level 2).
  - Default: `https://rtc.topstepx.com/hubs/market`

## Operation

### Starting the Bridge
The bridge is usually started automatically by `quanuxctl bridge` or the main server.
To verify functionality manually:
```bash
# In extensions/python/signalr_bridge
python src/supervisor.py
```

### Health Check
The Node runtime exposes a health endpoint:
```bash
curl http://localhost:8077/health
```
Response should include `"status": "ok"` and `"hubUrl"`.

### Debugging Connection Issues
1.  **Check URLs**: Ensure `QUANUX_SIGNALR_USER_HUB` is set correctly.
2.  **Check Token**: The bridge relies on `SIGNALR_ACCESS_TOKEN` which is usually refreshed and injected by the main `quanux-topstepx` auth flow. If authentication fails, the bridge cannot connect.
3.  **Logs**: Check `~/.quanux/logs/quanux-bridge-signalr.log`.

## Development
- **Node.js**: `src/node/index.mjs`. Uses `@microsoft/signalr` client. 
  - To install deps: `npm install` in `src/node`.
- **Modifying Events**: If new TopstepX events need to be tracked, update the `on(...)` listeners in `index.mjs`.
