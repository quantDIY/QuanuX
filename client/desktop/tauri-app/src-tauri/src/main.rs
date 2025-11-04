#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::{
  Manager,
  menu::{MenuBuilder, MenuItem},
  tray::{TrayIconBuilder, TrayIconEvent},
};

fn main() {
  tauri::Builder::default()
    .setup(|app| {
      // Tray menu
      let show = MenuItem::with_id(app, "show", "Show", true, None::<&str>)?;
      let quit = MenuItem::with_id(app, "quit", "Quit", true, None::<&str>)?;
      let menu = MenuBuilder::new(app)
        .item(&show)
        .item(&quit)
        .build()?;

      // Tray icon
      let _tray = TrayIconBuilder::new()
        .menu(&menu)
        .icon(app.default_window_icon().unwrap_or_default())
        .tooltip("QuanuX")
        .on_tray_icon_event(|tray, event| {
          if let TrayIconEvent::DoubleClick { .. } = event {
            let app = tray.app_handle();
            if let Some(w) = app.get_webview_window("main") {
              let _ = w.show();
              let _ = w.set_focus();
            }
          }
          Ok(())
        })
        .build(app)?;

      Ok(())
    })
    .on_menu_event(|app, ev| {
      match ev.id().as_ref() {
        "show" => {
          if let Some(w) = app.get_webview_window("main") {
            let _ = w.show();
            let _ = w.set_focus();
          }
        }
        "quit" => app.exit(0),
        _ => {}
      }
    })
    .run(tauri::generate_context!())
    .expect("error while running QuanuX desktop");
}
