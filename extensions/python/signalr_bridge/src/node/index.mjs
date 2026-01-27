import express from 'express';
import cors from 'cors';
import { WebSocketServer } from 'ws';
import { HubConnectionBuilder, LogLevel, HttpTransportType } from "@microsoft/signalr";

// Config
// Config
const PORT = process.env.QUANUX_BRIDGE_PORT || 8077;
const LOG_LEVEL = process.env.QUANUX_LOG_LEVEL || "Information";

// Defaults from Env
const DEFAULT_HUB_URL = process.env.SIGNALR_HUB_URL;
const USER_HUB_URL = process.env.SIGNALR_USER_HUB;
const MARKET_HUB_URL = process.env.SIGNALR_MARKET_HUB;
const DEFAULT_ACCESS_TOKEN = process.env.SIGNALR_ACCESS_TOKEN;

// State
let hubConnection = null;
let currentHubUrl = null;

// Express App
const app = express();
app.use(cors());
app.use(express.json());

// WebSocket Server (attached to same HTTP server later, or separate)
// For simplicity in this scaffold, we'll attach to the HTTP server instance.
let wss = null;

// --- Helper: Broadcast to WS Clients ---
const broadcast = (type, data) => {
  if (!wss) return;
  const msg = JSON.stringify({ type, data, timestamp: new Date().toISOString() });
  wss.clients.forEach(client => {
    if (client.readyState === 1) { // OPEN
      client.send(msg);
    }
  });
};

// --- API Endpoints ---

app.get('/health', (req, res) => {
  res.json({
    service: "quanux-bridge-node",
    status: "ok",
    connected: hubConnection ? hubConnection.state : "Disconnected",
    hubUrl: currentHubUrl,
    config: {
      default: DEFAULT_HUB_URL ? "Set" : "Unset",
      user: USER_HUB_URL ? "Set" : "Unset",
      market: MARKET_HUB_URL ? "Set" : "Unset"
    }
  });
});

app.post('/connect', async (req, res) => {
  let { hub_url, access_token, skip_negotiation } = req.body;

  // Resolve Aliases / Defaults
  if (!hub_url) hub_url = DEFAULT_HUB_URL;
  if (hub_url === "user" && USER_HUB_URL) hub_url = USER_HUB_URL;
  if (hub_url === "market" && MARKET_HUB_URL) hub_url = MARKET_HUB_URL;

  if (!access_token) access_token = DEFAULT_ACCESS_TOKEN;

  if (!hub_url) {
    return res.status(400).json({ error: "hub_url required (and no default set)" });
  }

  try {
    // Stop existing if any
    if (hubConnection) {
      await hubConnection.stop();
      hubConnection = null;
    }

    currentHubUrl = hub_url;

    let finalHubUrl = hub_url;

    // Build Options
    const options = {};

    if (skip_negotiation) {
      options.skipNegotiation = true;
      options.transport = HttpTransportType.WebSockets;
      if (access_token) {
        finalHubUrl += (finalHubUrl.includes("?") ? "&" : "?") + `access_token=${encodeURIComponent(access_token)}`;
      }
    } else {
      options.accessTokenFactory = access_token ? () => access_token : undefined;
    }

    // Build Connection
    const builder = new HubConnectionBuilder()
      .withUrl(finalHubUrl, options)
      .withAutomaticReconnect()
      .configureLogging(LogLevel[LOG_LEVEL] || LogLevel.Information);

    hubConnection = builder.build();

    // Bind Events
    hubConnection.onclose(err => {
      console.error("SignalR Closed:", err);
      broadcast("status", { state: "Disconnected", error: err?.message });
    });

    hubConnection.onreconnecting(err => {
      console.warn("SignalR Reconnecting:", err);
      broadcast("status", { state: "Reconnecting", error: err?.message });
    });

    hubConnection.onreconnected(id => {
      console.log("SignalR Reconnected:", id);
      broadcast("status", { state: "Connected", connectionId: id });
    });

    // GENERIC EVENT TRAP (We need to know what to listen to)
    // Since SignalR client doesn't support wildcard listener easily, 
    // we rely on 'subscribe' endpoint to add listeners dynamically OR hardcode common ones.
    // For Topstep, common events might be 'tick', 'price', etc. 
    // We will allow dynamic subscription via /subscribe.

    await hubConnection.start();
    console.log("Connected to", hub_url);
    broadcast("status", { state: "Connected" });

    res.json({ ok: true, message: "Connected" });

  } catch (e) {
    console.error("Connection Failed:", e);
    res.status(500).json({ error: e.message });
  }
});

app.post('/subscribe', (req, res) => {
  const { method, args } = req.body; // method to invoke on Server (e.g. Subscribe) which triggers client events? 
  // OR are we registering a client-side listener?

  // TOPSTEP PATTERN: 
  // Usually you Invoke "Subscribe" on server, and server pushes back messages via a specific event name (e.g. 'T' or 'marketData').

  if (!hubConnection || hubConnection.state !== "Connected") {
    return res.status(400).json({ error: "SignalR not connected" });
  }

  // 1. Invoke Server Method (if provided)
  if (method) {
    console.log("Invoking method:", method, "with args:", args);
    hubConnection.invoke(method, ...(args || []))
      .then(() => {
        // res.json({ ok: true, message: `Invoked ${method}` }); // We can return here or wait
      })
      .catch(err => {
        console.error("Invoke failed for method:", method, err);
        broadcast("error", { context: "invoke", method, error: err.message });
      });
  }

  // 2. Register Client Listener (if we know the response event name)
  // For this generic bridge, we might want an endpoint to register listeners. 
  // For now, let's hardcode a "catch-all" approach if possible or just common names.
  // For Topstep, market data often comes on specific channels. 
  // Let's assume the user tells us the Event Name to listen to in the BODY?

  // NOTE: For now, we'll assume we listen to 'T' (Trade/Tick) and 'Q' (Quote) and 'marketdata' by default or add a 'listener' param.
  const { listener } = req.body;
  if (listener) {
    console.log("Adding listener for:", listener);
    hubConnection.off(listener); // remove prev to avoid dupes
    hubConnection.on(listener, (...data) => {
      // Forward to WS
      broadcast("data", { event: listener, payload: data });
    });
  }

  res.json({ ok: true, message: "Subscription requested" });
});

// --- Start Server ---
const server = app.listen(PORT, () => {
  console.log(`Bridge (Express) running on port ${PORT}`);
});

// Attach WebSocket Server
wss = new WebSocketServer({ server });

wss.on('connection', (ws) => {
  console.log("WebSocket Client Connected");
  ws.send(JSON.stringify({ type: "welcome", message: "Connected to Quanux SignalR Bridge" }));

  if (hubConnection && hubConnection.state === "Connected") {
    ws.send(JSON.stringify({ type: "status", data: { state: "Connected" } }));
  }
});
