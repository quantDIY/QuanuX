---
name: quickfix_engine
description: High-performance Financial Information eXchange (FIX) engine for electronic trading.
---

# QuickFIX Engine

This extension provides the [QuickFIX](https://github.com/quickfix/quickfix) engine, compliant with **Rule 0 (Performance = C++)**.

## Features
-   **C++ Core**: Sub-millisecond latency for high-frequency trading.
-   **Python Bindings**: Scriptable control for Python strategies.
-   **Static Linking**: Portable execution across Execution Nodes.

## Usage (Python Strategy)

```python
import quickfix as fix

class Application(fix.Application):
    def onCreate(self, sessionID):
        print("Session created:", sessionID)
    
    def onLogon(self, sessionID):
        print("Logon:", sessionID)

    def onLogout(self, sessionID):
        print("Logout:", sessionID)

    def toAdmin(self, message, sessionID):
        pass

    def fromAdmin(self, message, sessionID):
        pass

    def toApp(self, message, sessionID):
        pass

    def fromApp(self, message, sessionID):
        print("Received message:", message)

# Configuration
settings = fix.SessionSettings("config.ini")
application = Application()
storeFactory = fix.FileStoreFactory(settings)
logFactory = fix.FileLogFactory(settings)
initiator = fix.SocketInitiator(application, storeFactory, settings, logFactory)

# Start
initiator.start()
# ... keep running ...
initiator.stop()
```

## Usage (C++ Execution Node)
The C++ SDK (headers and static libs) is installed in `extensions/cpp/quickfix/dist`.
Link against `libquickfix.a` and include headers from `include/`.

## Management

**Install (Latest)**
```bash
quanuxctl install quickfix
```

**Install (Specific Version)**
```bash
quanuxctl install quickfix -v v1.14.0
```

**Check for Updates**
```bash
quanuxctl upgradeable quickfix
```

**Auto-Upgrade**
```bash
quanuxctl upgrade quickfix
```

**Uninstall**
```bash
quanuxctl remove quickfix
```
