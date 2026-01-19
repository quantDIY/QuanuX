---
name: rithmic-broker-guide
description: Detailed guide for configuring Rithmic connections for various brokers and prop firms.
---

# Rithmic Broker Configuration Guide

This guide helps Agents assist users in connecting their specific Broker or Prop Firm account to the QuanuX Rithmic Extension.

## 1. Topstep (Prop Firm)
*   **System Name**: `Topstep`
*   **Gateway**: Chicago (Aurora)
*   **Username Format**: Usually matches the Rithmic user (not the website login).
*   **Nuances**:
    *   Topstep accounts often block "R | Trader Pro" plugins. Direct connection is preferred.
    *   Data is often Top of Book (Aggregated) unless Level 2 is purchased.

## 2. Apex Trader Funding (Prop Firm)
*   **System Name**: `Apex`
*   **Gateway**: Chicago (Aurora)
*   **User Constraints**:
    *   **Strict concurrency limits**: Apex aggressively disconnects multiple sessions.
    *   **Recommendation**: Users MUST log out of R | Trader / NinjaTrader before connecting QuanuX, OR use an extra paid data connection.

## 3. AMP Futures (Retail Broker)
*   **System Name**: `AMP`
*   **Gateway**: Chicago (Aurora) or Europe depending on residency.
*   **Nuances**:
    *   Standard retail accounts. Very stable.
    *   Supports full Market Depth (MBO) if subscribed.

## 4. Edge Clear (Retail Broker)
*   **System Name**: Depends on FCM. Check the clearing statement.
    *   If **Phillips**: Use `Phillips`
    *   If **Ironbeam**: Use `Ironbeam`
    *   If **Dorman**: Use `Dorman`
*   **Gateway**: Chicago.

## 5. Optimus Futures (Retail Broker)
*   **System Name**: Often `Optimus` or the underlying FCM (`Ironbeam`).
*   **Gateway**: Chicago.

## Agent Instructions: Diagnosing Connection Issues
1.  **"Invalid Login/Password"**:
    *   Ask user: "Are you using your dashboard login or your specific Rithmic credentials?" (They are often different).
    *   Ask user: "Is the System Name correct for your broker?"
2.  **"Session Disconnected"**:
    *   Ask user: "Do you have R | Trader Pro or NinjaTrader open?"
    *   Remind them of the single-session limit for most prop firms.
3.  **"No Market Data"**:
    *   Check if they selected the correct gateway.
    *   Verify their data subscription covers the symbol (e.g., "Do you have the CME Bundle?").
