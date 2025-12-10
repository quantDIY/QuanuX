// SAMPLE ONLY — replace during wiring
import { HubConnectionBuilder, LogLevel } from "@microsoft/signalr";

const HUB_URL = process.env.SIGNALR_HUB_URL || "https://example.invalid/hub";
const TOKEN = process.env.SIGNALR_ACCESS_TOKEN || null;

const connection = new HubConnectionBuilder()
  .withUrl(HUB_URL, TOKEN ? { accessTokenFactory: () => TOKEN } : undefined)
  .withAutomaticReconnect()
  .configureLogging(LogLevel.Information)
  .build();

connection.onclose(err => {
  console.error(JSON.stringify({ type: "signalr.close", error: (err && err.message) || null }));
});

connection.onreconnecting(err => {
  console.warn(JSON.stringify({ type: "signalr.reconnecting", error: (err && err.message) || null }));
});

connection.onreconnected(id => {
  console.log(JSON.stringify({ type: "signalr.reconnected", id }));
});

// Example event subscription — customize per provider during wiring
const subscribe = () => {
  // connection.on("tick", data => console.log(JSON.stringify({ type: "tick", data })));
};

(async function main() {
  try {
    await connection.start();
    console.log(JSON.stringify({ type: "signalr.ready" }));
    subscribe();

    // Keep process alive
    setInterval(() => { }, 10000);
  } catch (e) {
    console.error(JSON.stringify({ type: "signalr.error", error: e.message }));
    process.exit(1);
  }
})();

// Minimal stub removed. Main logic above will run.
// console.log("[SignalR Node Bridge] scaffold online (no-op)");
// setInterval(() => process.stdout.write("."), 1000);
