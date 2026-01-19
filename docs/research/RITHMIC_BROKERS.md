# Research: Rithmic Broker Connectivity

## Core Concept: FCM vs. Introducing Broker
For Rithmic, the critical connection parameter is the **System Name**. This usually corresponds to the **FCM (Futures Commission Merchant)** holding the funds, *not* necessarily the **Introducing Broker (IB)** the user interacts with (though some large IBs have their own System IDs).

## System Name Mapping
To connect successfully, users must map their Broker/Funding Program to the correct Rithmic System Name.

| Broker / Funding Program | Likely FCM / Backend | **Rithmic System Name** (Key) | Nuances |
| :--- | :--- | :--- | :--- |
| **Rithmic Paper** | *Internal* | `Rithmic Paper Trading` | Public demo env. Good for testing. |
| **AMP Futures** | AMP Global | `AMP` | Direct connection. Very common. |
| **Topstep** | *Prop / Enterprise* | `Topstep` | Often requires specific Rithmic user permissions. |
| **Apex Trader Funding** | *Prop / Enterprise* | `Apex` | High volume, strict connection limits so use `plugin` mode if running coincidentally with R Trader. |
| **Optimus Futures** | Ironbeam / Wedbush | `Ironbeam` / `WEDBUSH` | Depends on the clearing arrangement. |
| **Edge Clear** | Phillips / Dorman | `Phillips` / `Dorman` | User must check their clearing statement. |
| **Leeloo Trading** | *Prop* | `Leeloo` | |
| **TickTick Trader** | *Prop* | `TickTick` | |
| **UProfit** | *Prop* | `UProfit` | |

## Connectivity Nuances

### 1. The Gateway (Region)
Rithmic has regionally optimized gateways. Latency depends heavily on choosing the right one.
*   **Chicago (Aurora DC3)**: The primary liquidity hub.
    *   URI: `ritmms.rithmic.com:6500` (SSL)
*   **Europe (Frankfurt)**: `ritmms-eu.rithmic.com`
*   **Asia (Tokyo/Singapore)**: `ritmms-asia.rithmic.com`

**Guidance**: Bare-metal Execution Nodes should be physically located in **Aurora (Chicago)** for US Futures to minimize TCP round-trip to the Rithmic Gateway.

### 2. Plug-in Mode vs. Direct Mode
*   **Direct Mode**: The API connects directly to Rithmic servers.
    *   *Pros*: Lowest latency.
    *   *Cons*: Consumes a "concurrent session" slot. If the user logs into R | Trader Pro simultaneously without paying for extra sessions, one will disconnect.
*   **Plug-in Mode**: The API connects *through* a running instance of R | Trader Pro.
    *   *Pros*: Shares the session (no extra fee).
    *   *Cons*: Higher latency (loopback), requires Windows (usually) or Wine. **Not recommended for Linux Bare-Metal Execution Nodes.**

### 3. SSL Certificates
Rithmic's C++/Go APIs often require a local path to the heartbeat/SSL certificate bundle (`rithmic_ssl_cert.pem`).
*   **Action for Extensions**: We must bundle a fresh set of these certs or provide a mechanism to download them dynamically during `quanux-node` setup.

### 4. Market Data Subscription (MBO vs. Aggregated)
*   **MBO (Market By Order)**: Full depth. Huge bandwidth usage. Requires robust handling in the Agent/Algorithm.
*   **Aggregated**: Top of Book. Lower bandwidth.
*   *Note*: Some prop firms (Apex/Topstep) provide Level 1 (Top of Book) by default to save costs. The Extension must handle "missing" MBO data gracefully.

## Proposed "Broker Integration" Strategy
Don't write separate Extensions for "AMP" vs "Apex". Write **One Rithmic Extension** with a **Configuration Wizard**.

**Extensions Config (`extension.yaml`)**:
```yaml
config:
  - name: "system_name"
    type: "select"
    options: ["Rithmic Paper Trading", "AMP", "Topstep", "Apex", "Ironbeam", "Custom"]
  - name: "gateway"
    type: "select"
    options: ["Chicago", "Europe", "Asia"]
```

This keeps the codebase DRY while supporting the entire ecosystem.
