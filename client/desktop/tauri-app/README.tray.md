# QuanuX Desktop — Ubuntu GNOME Tray, Multi-Window Hooks, and Event Contracts

This wiring provides:
- **System tray** with menu actions
- **Notifications** via `tauri-plugin-notification`
- **Window plumbing** (multi-window, no UI design yet)
- **Event contracts** for React to handle later

## Events emitted to React
- `quanux://new-tab` — open tab in current window
- `quanux://new-window` — new window created (payload: label)
- `quanux://select-server` — open server picker (local vs remote)
- `quanux://select-instance` — choose active instance (multi PID/port)
- `quanux://reconnect` — reconnect bridge/websocket

## Run
    cd client/desktop/tauri-app
    npm i
    npm run tauri:dev

## Optional: Ubuntu launcher / autostart
    bash meta/scripts/install_desktop_entry.sh

## Replace icons
Put your PNGs in `src-tauri/icons/` and update `tauri.conf.json` if needed.
