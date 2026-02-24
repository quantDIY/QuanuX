#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod market_tick;

use async_nats;
use bytemuck;
use futures::StreamExt;
use market_tick::MarketTick;
use tauri::{Manager, Emitter};
use tauri::tray::TrayIconBuilder;

fn main() {
  tauri::Builder::default()
    .setup(|app| {
      let handle = app.handle().clone();

      // Spawn the high-frequency UI background telemetry task
      tauri::async_runtime::spawn(async move {
          // Connect to the local NATS QuanuX fabric
          if let Ok(client) = async_nats::connect("nats://localhost:4222").await {
              if let Ok(mut subscriber) = client.subscribe("MARKET.BIN").await {
                  while let Some(msg) = subscriber.next().await {
                      // Validate 64-byte L1 Cache Line rule before parsing
                      if msg.payload.len() == std::mem::size_of::<MarketTick>() {
                          if let Ok(tick) = bytemuck::try_from_bytes::<MarketTick>(&msg.payload) {
                              // Broadcast the decoded telemetry (TSC included) to the React GUI
                              let _ = handle.emit("market-tick", tick);
                          }
                      }
                  }
              }
          }
      });

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
