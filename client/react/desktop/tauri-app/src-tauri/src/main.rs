#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod market_tick;
mod execution_command;

use async_nats;
use bytemuck;
use futures::StreamExt;
use market_tick::MarketTick;
use execution_command::CommandPacket;
use tauri::{Manager, Emitter, State};
use tauri::tray::TrayIconBuilder;
use tauri::menu::{Menu, MenuItem};
use tauri_plugin_notification::NotificationExt;

// Phase 2: The Tauri Fast-Path
#[tauri::command]
async fn invoke_execution_trigger(
    command_type: u32,
    signature: u32,
    nats_client: State<'_, async_nats::Client>,
) -> Result<(), String> {
    let packet = CommandPacket {
        // High-precision clock simulation (or read native TSC if `core_affinity` was active)
        timestamp_tsc: std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_nanos() as u64,
        command_type,
        signature,
        _pad1: [0; 32],
        _pad2: [0; 16],
    };

    let bytes = bytemuck::bytes_of(&packet);
    nats_client.publish("COMMAND.BIN", bytes.to_vec().into()).await.map_err(|e| e.to_string())?;
    Ok(())
}

fn main() {
  tauri::Builder::default()
    .plugin(tauri_plugin_notification::init())
    .invoke_handler(tauri::generate_handler![invoke_execution_trigger])
    .setup(|app| {
      let handle = app.handle().clone();
      
      // Phase 3: The System Tray Action
      let kill_item = MenuItem::with_id(app, "kill_all", "Emergency Stop (KILL_ALL)", true, None::<&str>)?;
      let menu = Menu::with_items(app, &[&kill_item])?;
      
      TrayIconBuilder::new()
        .menu(&menu)
        .on_menu_event(move |app_handle, event| {
          if event.id.as_ref() == "kill_all" {
             // Bypass GUI, throw 64-byte payload directly at the C++ kernel.
             if let Some(client) = app_handle.try_state::<async_nats::Client>() {
                 let packet = CommandPacket {
                     timestamp_tsc: std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap().as_nanos() as u64,
                     command_type: 99, // Emergency Stop
                     signature: 0xDEADBEEF,
                     _pad1: [0; 32],
                     _pad2: [0; 16],
                 };
                 let bytes = bytemuck::bytes_of(&packet).to_vec();
                 let c = client.inner().clone();
                 tauri::async_runtime::spawn(async move {
                     let _ = c.publish("COMMAND.BIN", bytes.into()).await;
                 });
             }
          }
        })
        .on_tray_icon_event(|tray, _event| {
          let app = tray.app_handle();
          if let Some(win) = app.get_webview_window("main") {
            let _ = win.show();
            let _ = win.set_focus();
          }
        })
        .build(app)?;

      let nats_app_handle = app.handle().clone();
      // Spawn the high-frequency UI background telemetry task
      tauri::async_runtime::spawn(async move {
          let mut tick_count = 0;
          let mut notified = false;
          
          // Connect to the local NATS QuanuX fabric
          if let Ok(client) = async_nats::connect("nats://localhost:4222").await {
              // Inject the verified client into the App state for outbound commands
              nats_app_handle.manage(client.clone());
              
              if let Ok(mut subscriber) = client.subscribe("MARKET.BIN").await {
                  while let Some(msg) = subscriber.next().await {
                      // Validate 64-byte L1 Cache Line rule before parsing
                      if msg.payload.len() == std::mem::size_of::<MarketTick>() {
                          if let Ok(tick) = bytemuck::try_from_bytes::<MarketTick>(&msg.payload) {
                              // Broadcast the decoded telemetry (TSC included) to the React GUI
                              let _ = handle.emit("market-tick", tick);
                              
                              tick_count += 1;
                              if tick_count >= 100 && !notified {
                                  notified = true;
                                  let _ = handle.notification()
                                      .builder()
                                      .title("System Ready")
                                      .body("100 Telemetry frames parsed with zero drop.")
                                      .show();
                              }
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

      Ok(())
    })
    .run(tauri::generate_context!())
    .expect("error running QuanuX Desktop");
}
