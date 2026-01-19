---
name: rithmic-extension-dev
description: Specialized knowledge for developing the QuanuX Rithmic Extension. Covers Protocol Buffers, 2-step Login, and Go implementation details.
metadata:
  version: 1.0.0
  author: QuanuX
---

# Rithmic Extension Developer Guide

This skill grants you the specific knowledge needed to maintain and extend the Rithmic Go client.

## 1. The Protocol (Proto2 vs Proto3)

The official Rithmic definitions are **Proto2**. This has major implications for Go code:
-   **Pointers**: All fields are pointers (e.g., `*string`, `*int32`).
-   **Access**: You cannot access fields directly like `msg.Price`. You must check for nil or use helpers.
-   **Assignment**: Use `proto.String("val")`, `proto.Int32(10)`, etc.

**Pro-Tip**: Always verify `msg != nil` and `msg.TemplateId != nil` before dereferencing.

## 2. Message Routing (Template IDs)

Rithmic multiplexes all messages over a single WebSocket. You must route based on `TemplateId`.

```go
// Constants
const (
    MSG_LOGIN = 10
    MSG_MARKET_DATA = 100
)

// Routing Logic
if *msg.TemplateId == MSG_LOGIN { ... }
```

Refer to `docs/RITHMIC_INTEGRATION.md` for the Table of IDs.

## 3. The "Two-Step" Dance

**Do not try to login immediately.** You will be rejected.
1.  **Probe**: Connect -> `RequestRithmicSystemInfo` -> Get Systems -> Disconnect.
2.  **Login**: Connect -> `RequestLogin` (using a valid system name).

## 4. Maintenance Tasks

### Updating the API
1.  Drop new `.proto` files into `extensions/rithmic/proto/`.
2.  Run `patch_protos_robust.py` (if available) or ensure `option go_package = "quant/extensions/rithmic/api";` is in every file.
3.  Run `protoc` (see `docs/RITHMIC_INTEGRATION.md` for exact command).

### Testing
Use the built-in CLI command to verify connectivity:
```bash
quanuxctl ext run rithmic
```
This injects secrets and runs `main.go` directly.

## 5. Common Gotchas
-   **Heartbeats**: Failing to send Template 18 every 30s will drop the connection.
-   **Framing**: Forgot the 4-byte Big Endian header? server ignores you.
-   **SSL**: Must use `wss://`.
