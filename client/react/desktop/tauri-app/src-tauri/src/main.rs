#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::Manager;
use tauri::tray::TrayIconBuilder;

fn main() {
  tauri::Builder::default()
    .setup(|app| {
      // Ensure a main window exists
      if app.get_webview_window("main").is_none() {
        tauri::WebviewWindowBuilder::new(
          app,
          "main",
          tauri::WebviewUrl::App("index.html".into()),
        )
        .title("QuanuX Desktop")
        .inner_size(1200.0, 800.0)
        .resizable(true)
        .build()?;
      }

      // Build tray WITHOUT loading an icon in code.
      // Icon is provided by tauri.conf.json => app.trayIcon.iconPath
      TrayIconBuilder::new()
        .on_tray_icon_event(|tray, _event| {
          let app = tray.app_handle();
          if let Some(win) = app.get_webview_window("main") {
            let _ = win.show();
            let _ = win.set_focus();
          }
        })
        .build(app)?;

      Ok(())
    })
    .run(tauri::generate_context!())
    .expect("error running QuanuX Desktop");
}
