#!/usr/bin/env bash
set -euo pipefail

# Run from repo root
test -d client/desktop/tauri-app/src-tauri || {
  echo "Run from repo root (missing client/desktop/tauri-app/src-tauri)"
  exit 1
}

APP_ROOT="client/desktop/tauri-app"
TAURI_ROOT="$APP_ROOT/src-tauri"
ICONS_DIR="$TAURI_ROOT/icons"
SRC_DIR="$TAURI_ROOT/src"

mkdir -p "$ICONS_DIR" "$SRC_DIR" "meta/scripts" "meta/manifests/desktop"

# 1) tray.rs — GNOME tray + menu + signals
cat >"$SRC_DIR/tray.rs"<<'RS'
use tauri::{Manager, Result};
use tauri::tray::{TrayIconBuilder, TrayIcon, Menu, MenuItem};
use tauri_plugin_notification::NotificationExt;
use tauri::WebviewWindowBuilder;
use std::time::{SystemTime, UNIX_EPOCH};

fn unique_label(prefix: &str) -> String {
  let nanos = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos();
  format!("{}-{}", prefix, nanos)
}

pub struct TrayHandles {
  pub tray: TrayIcon,
}

fn ensure_main(app: &tauri::AppHandle) {
  if app.get_webview_window("main").is_none() {
    let _ = WebviewWindowBuilder::new(app, "main", tauri::WebviewUrl::App("index.html".into()))
      .title("QuanuX Desktop")
      .inner_size(1200.0, 800.0)
      .resizable(true)
      .build();
  }
}

fn spawn_new_window(app: &tauri::AppHandle) {
  let label = unique_label("win");
  if WebviewWindowBuilder::new(app, &label, tauri::WebviewUrl::App("index.html".into()))
      .title("QuanuX Desktop")
      .inner_size(1200.0, 800.0)
      .resizable(true)
      .build().is_ok() {
    let _ = app.emit("quanux://new-window", &label);
    let _ = app.notification().notify("QuanuX", "Opened a new window", None);
  }
}

pub fn init_tray(app: &tauri::AppHandle) -> Result<TrayHandles> {
  let menu = Menu::new()
    .add_item(MenuItem::with_id("show", "Show Window", true, None::<&str>))
    .add_item(MenuItem::with_id("new_tab", "New Tab", true, None::<&str>))
    .add_item(MenuItem::with_id("new_window", "New Window", true, None::<&str>))
    .add_native_item(MenuItem::Separator)
    .add_item(MenuItem::with_id("select_server", "Select Server...", true, None::<&str>))
    .add_item(MenuItem::with_id("select_instance", "Select Instance...", true, None::<&str>))
    .add_item(MenuItem::with_id("reconnect", "Reconnect", true, None::<&str>))
    .add_native_item(MenuItem::Separator)
    .add_item(MenuItem::with_id("quit", "Quit", true, None::<&str>));

  let tray = TrayIconBuilder::new()
    .icon(tauri::image::Image::from_path(app, "icons/tray-light.png")?)
    .menu(&menu)
    .tooltip("QuanuX Desktop")
    .on_menu_event(|app, event| {
      match event.id.as_ref() {
        "show" => {
          ensure_main(app);
          if let Some(w) = app.get_webview_window("main") {
            let _ = w.show();
            let _ = w.set_focus();
          }
        }
        "new_tab" => {
          ensure_main(app);
          let _ = app.emit("quanux://new-tab", ());
          let _ = app.notification().notify("QuanuX", "New tab requested", None);
        }
        "new_window" => { spawn_new_window(app); }
        "select_server" => {
          ensure_main(app);
          let _ = app.emit("quanux://select-server", ());
        }
        "select_instance" => {
          ensure_main(app);
          let _ = app.emit("quanux://select-instance", ());
        }
        "reconnect" => { let _ = app.emit("quanux://reconnect", ()); }
        "quit" => { std::process::exit(0); }
        _ => {}
      }
    })
    .on_tray_icon_event(|app, event| {
      if event.click.is_some() {
        ensure_main(app);
        if let Some(w) = app.get_webview_window("main") {
          if w.is_visible().unwrap_or(true) { let _ = w.hide(); }
          else { let _ = w.show(); let _ = w.set_focus(); }
        }
      }
    })
    .build(app)?;

  Ok(TrayHandles { tray })
}
RS

# 2) main.rs — initialize tray and notification
cat >"$SRC_DIR/main.rs"<<'RS'
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod tray;
use tray::init_tray;

fn main() {
  tauri::Builder::default()
    .plugin(tauri_plugin_notification::init())
    .setup(|app| {
      if app.get_webview_window("main").is_none() {
        tauri::WebviewWindowBuilder::new(app, "main", tauri::WebviewUrl::App("index.html".into()))
          .title("QuanuX Desktop")
          .inner_size(1200.0, 800.0)
          .resizable(true)
          .build()?;
      }
      let _handles = init_tray(&app.handle())?;

      // Event contract doc:
      //   quanux://new-tab
      //   quanux://new-window
      //   quanux://select-server
      //   quanux://select-instance
      //   quanux://reconnect
      Ok(())
    })
    .run(tauri::generate_context!())
    .expect("error while running QuanuX desktop");
}
RS

# 3) Icon placeholders
cat >"$ICONS_DIR/tray-light.png"<<'PNG'
iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAAF0lEQVR4nO3MMQEAIAwDsQx3/2oQJqQd1Nw9J0gP1gAA6f1a4QAAAABJRU5ErkJggg==
PNG
cp "$ICONS_DIR/tray-light.png" "$ICONS_DIR/tray-dark.png"

# 4) Cargo deps
CARGO="$TAURI_ROOT/Cargo.toml"
awk '
  BEGIN{added=0}
  /^\[dependencies\]/ && !added {
    print; print "tauri = { version = \"2\", features = [\"tray-icon\"] }";
    print "serde = { version = \"1\", features = [\"derive\"] }";
    print "serde_json = \"1\"";
    print "tauri-plugin-notification = \"2\"";
    added=1; next
  }1' "$CARGO" > "$CARGO.tmp" && mv "$CARGO.tmp" "$CARGO"

# 5) tauri.conf.json patch
CONF="$TAURI_ROOT/tauri.conf.json"
if ! grep -q '"systemTray"' "$CONF"; then
  sed -i 's|"app": {|"app": { "windows": [{ "title": "QuanuX", "width": 1200, "height": 800 }], "systemTray": { "iconPath": "icons/tray-light.png", "iconAsTemplate": false },|g' "$CONF" || true
fi

# 6) .desktop launcher + autostart
cat >"meta/manifests/desktop/quanux-desktop.desktop"<<'DESK'
[Desktop Entry]
Type=Application
Name=QuanuX Desktop
Comment=QuanuX Desktop (Tauri Shell)
Exec=/usr/bin/env bash -lc 'cd "$HOME/Projects/QuanuX/quanux-full-context-v1/client/desktop/tauri-app" && npm run tauri:dev'
Icon=utilities-terminal
Terminal=false
Categories=Finance;Utility;Development;
StartupWMClass=QuanuX
DESK

cat >"meta/scripts/install_desktop_entry.sh"<<'SH'
#!/usr/bin/env bash
set -euo pipefail
SRC="meta/manifests/desktop/quanux-desktop.desktop"
TARGET="${XDG_DATA_HOME:-$HOME/.local/share}/applications/quanux-desktop.desktop"
mkdir -p "$(dirname "$TARGET")"
cp "$SRC" "$TARGET"
update-desktop-database "$(dirname "$TARGET")" >/dev/null 2>&1 || true
echo "Installed launcher: $TARGET"

AUTOSTART="${XDG_CONFIG_HOME:-$HOME/.config}/autostart"
mkdir -p "$AUTOSTART"
cp "$SRC" "$AUTOSTART/quanux-desktop.desktop"
sed -i 's/npm run tauri:dev/npm run tauri:dev \& disown/g' "$AUTOSTART/quanux-desktop.desktop" || true
echo "Autostart entry: $AUTOSTART/quanux-desktop.desktop"
SH
chmod +x "meta/scripts/install_desktop_entry.sh"

# 7) README for developers
cat >"$APP_ROOT/README.tray.md"<<'MD'
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
MD

echo
echo "==> Ubuntu tray + notifications + multi-window hooks + event contracts installed."
echo "Next:"
echo "  1) cd client/desktop/tauri-app && npm i"
echo "  2) npm run tauri:dev"
echo "  3) (Optional) bash meta/scripts/install_desktop_entry.sh"
echo "  4) Handle emitted events in React ('quanux://…')."
echo
