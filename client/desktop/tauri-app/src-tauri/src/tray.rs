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
