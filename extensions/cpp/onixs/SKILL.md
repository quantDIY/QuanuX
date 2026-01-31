---
name: onixs-proprietary-injection
description: Protocol for securely integrating the OnixS High-Performance FIX Engine into QuanuX via local injection.
version: 1.0.0
---

# Service Protocol: OnixS Proprietary Injection

This document defines the architecture used to integrate **OnixS** (a commercial, proprietary library) into **QuanuX** (an open-source platform) without violating IP rights or distributing restricted code.

## 1. The "Zero-Persistence" Architecture

QuanuX does not ship with OnixS. Instead, it ships with a **Shim**: a hollow adapter designed to fit the OnixS shape perfectly.

1.  **User Acquires SDK**: The user legally downloads the SDK from the OnixS Client Portal.
2.  **User Injects SDK**: `quanuxctl integrate onixs <path>` copies the headers/libs to `vendor/`.
3.  **QuanuX Compiles**: The build system detects the presence of `vendor/include/OnixS/FixEngine.h` and activates the compilation of the Shim.

## 2. Directory Structure

```text
extensions/cpp/onixs/
├── extension.yaml      # Manifest
├── .gitignore          # 🔒 IGNORES vendor/
├── vendor/             # 🚫 PROPRIETARY (Injected at Runtime)
│   ├── include/
│   └── lib/
└── shim/               # ✅ OPEN SOURCE
    └── OnixSAdapter.hpp # Compiles against vendor/
```

## 3. Integration Workflow

```bash
# 1. Download SDK to Desktop
# 2. Integrate
quanuxctl integrate onixs ~/Desktop/OnixS.FixEngineCpp-Ubuntu2204...

# 3. Build Extension
quanuxctl ext build onixs
```

## 4. IP Protection Guarantees

*   **No Distribution**: Proprietary code is never staged, committed, or pushed.
*   **Local Only**: The integration exists only on the licensed user's machine.
*   **Git-Ignored**: The `.gitignore` at the extension root explicitly blocks `vendor/`.
