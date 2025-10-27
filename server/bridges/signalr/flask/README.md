# SignalR Bridge (Flask) — Scaffold

Purpose: a local HTTP surface QuanuX Server can control that, in the future,
manages ASP.NET Core SignalR hub connections (negotiate, connect, subscribe, dispatch).

**Status:** scaffold only — no live SignalR traffic.

## Endpoints
- `GET /health` — probe
- `POST /connect` — accept config, will negotiate a hub later
- `POST /subscribe` — accept stream prefs
- `POST /disconnect` — release connection

## Env
- `QUANUX_BRIDGE_PORT` (default `8077`)
- `QUANUX_LOG_LEVEL` (default `INFO`)
- `QUANUX_BRIDGE_CORS` (default `*`)

Run directly:
```bash
export QUANUX_BRIDGE_PORT=8077
python3 server/bridges/signalr/flask/app.py
