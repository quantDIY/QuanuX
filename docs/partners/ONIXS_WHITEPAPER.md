# QuanuX & OnixS: Secure Integration Whitepaper

**To:** Leadership & Engineering Analysis, OnixS  
**From:** QuanuX Architecture Team  
**Subject:** Zero-Persistence IP Protection & Native Integration Architecture

---

## 1. Executive Summary

QuanuX has implemented a "Proprietary Injection" architecture designed specifically to accommodate high-performance commercial libraries like **OnixS** within an open-source decentralized platform.

This architecture guarantees:
1.  **Zero Persistence**: OnixS proprietary code (headers, libraries) is **never** committed to the QuanuX repository.
2.  **License Compliance**: The user is required to "Bring Your Own License" (BYOL) and acquire the SDK directly from OnixS.
3.  **Native Performance**: Despite the decoupled distribution, QuanuX compiles directly against the OnixS C++ libraries, incurring zero runtime overhead.

## 2. The "Stub & Shim" Architecture

We have introduced a new extension type: `proprietary-injection`.

### 2.1 The Stub (Public)
The QuanuX repository contains a "Hollow Extension" for OnixS:
*   `extensions/cpp/onixs/extension.yaml`: Metadata defining the dependency.
*   `extensions/cpp/onixs/shim/`: Detailed C++ adapters that *would* compile if the SDK were present.
*   `.gitignore`: **Strictly blocks** `vendor/`, ensuring no injected code can be tracked by git.

### 2.2 The Injection (Local Runtime)
The user executes a secure CLI command to "hydrate" the extension:

```bash
quanuxctl integrate onixs ~/Downloads/OnixS.FixEngineCpp-Ubuntu2204...
```

This command:
1.  **Verifies** the digital signature/structure of the SDK.
2.  **Transfers** headers and libs to the git-ignored `vendor/` directory.
3.  **Activates** the build system to link the Shim against the now-present SDK.

## 3. IP Protection Mechanisms

We employ a "Defense in Depth" strategy to protect OnixS IP:

| Layer | Mechanism | Result |
| :--- | :--- | :--- |
| **1. Git** | `.gitignore` rules for `vendor/` | Impossible to commit SDK files to version control. |
| **2. CLI** | `quanuxctl integrate` | Automates the secure placement of files, preventing user error/misplacement. |
| **3. Build** | `CMake` dynamic detection | Build only succeeds if valid, licensed SDK headers are found locally. |

## 4. Conclusion

This architecture allows QuanuX to offer "First Class" support for OnixS without redistributing restricted code. It transforms QuanuX into a compliant, high-performance runtime for OnixS strategies, respecting both the engineering constraints of HFT and the legal constraints of commercial software.
