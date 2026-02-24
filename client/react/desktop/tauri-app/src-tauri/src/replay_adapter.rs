use serde::{Deserialize, Serialize};
use std::time::Duration;
use tauri::{AppHandle, Emitter};
use tokio::time;

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct L3Snapshot {
    pub best_bid: f32,
    pub best_ask: f32,
    pub alpha: f32,
    pub tsc_lo: u32,
    pub execution_state: u8,
}

/// The ReplayAdapter directly taps the SovereignState struct mapped in L3 memory.
/// In this implementation it simulates scanning that pinned HugePage and emitting
/// the binary findings to the React Neural DOM via Tauri IPC.
pub struct ReplayAdapter;

impl ReplayAdapter {
    pub fn start_hardware_tap(app: AppHandle) {
        tauri::async_runtime::spawn(async move {
            let mut interval = time::interval(Duration::from_millis(150)); // Simulating 150ms UI painting framerate
            
            loop {
                interval.tick().await;

                // SIMULATING: Reading the 16-byte `L3Snapshot` from the 64-byte `SovereignState` memory map
                // Introduce Chaos Protocol: Simulate HALT every ~100 frames to verify the UI shield
                let is_chaos = rand::random::<u8>() % 100 < 5; // 5% chance of HALT
                let hardware_tick = L3Snapshot {
                    best_bid: 95000.5 + (rand::random::<f32>() * 50.0 - 25.0),
                    best_ask: 95001.0 + (rand::random::<f32>() * 50.0 - 25.0),
                    alpha: (rand::random::<f32>() * 2.0) - 1.0, // Alpha between -1.0 and 1.0
                    tsc_lo: rand::random::<u32>(),
                    execution_state: if is_chaos { 4 } else { 1 }, // 4 = HALT, 1 = WORKING
                };

                // The Visual Flight Recorder casts the exact C++ physical variables into JSON
                let _ = app.emit("l3-telemetry-tap", hardware_tick);
            }
        });
    }
}
