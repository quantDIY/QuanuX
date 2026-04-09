tauri-v2 by nodnarbnitram/claude-code-extensions

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [nodnarbnitram](/nodnarbnitram)/ [claude-code-extensions](/nodnarbnitram/claude-code-extensions)/tauri-v2

# tauri-v2

`$ npx skills add https://github.com/nodnarbnitram/claude-code-extensions --skill tauri-v2`

Summary

**Cross-platform desktop and mobile apps with Rust backends and web frontends.**

- Handles Tauri command registration, IPC patterns (invoke, emit, channels), and state management with built-in error prevention for 8+ common setup mistakes
- Requires explicit capability configuration in `capabilities/default.json` for all operations; Tauri v2 denies permissions by default
- Supports async commands with owned types, event emission, streaming channels, and proper error serialization patterns
- Covers configuration of `tauri.conf.json`, `Cargo.toml` library setup for mobile builds, and troubleshooting for white-screen launches, permission errors, and build failures

SKILL.md

# Tauri v2 Development Skill

> Build cross-platform desktop and mobile apps with web frontends and Rust backends.

## Before You Start

**This skill prevents 8+ common errors and saves ~60% tokens.**

MetricWithout SkillWith SkillSetup Time~2 hours~30 minCommon Errors8+0Token UsageHigh (exploration)Low (direct patterns)

### Known Issues This Skill Prevents

1. Permission denied errors from missing capabilities
2. IPC failures from unregistered commands in `generate_handler!`
3. State management panics from type mismatches
4. Mobile build failures from missing Rust targets
5. White screen issues from misconfigured dev URLs

## Quick Start

### Step 1: Create a Tauri Command

```rust code-highlight
// src-tauri/src/lib.rs
#[tauri::command]
fn greet(name: String) -> String {
    format!("Hello, {}!", name)
}

pub fn run() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![greet])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

```

**Why this matters:** Commands not in `generate_handler![]` silently fail when invoked from frontend.

### Step 2: Call from Frontend

```typescript code-highlight
import { invoke } from '@tauri-apps/api/core';

const greeting = await invoke<string>('greet', { name: 'World' });
console.log(greeting); // "Hello, World!"

```

**Why this matters:** Use `@tauri-apps/api/core` (not `@tauri-apps/api/tauri` \- that's v1 API).

### Step 3: Add Required Permissions

```json code-highlight
// src-tauri/capabilities/default.json
{
    "$schema": "../gen/schemas/desktop-schema.json",
    "identifier": "default",
    "windows": ["main"],
    "permissions": ["core:default"]
}

```

**Why this matters:** Tauri v2 denies everything by default - explicit permissions required for all operations.

## Critical Rules

### Always Do

- Register every command in `tauri::generate_handler![cmd1, cmd2, ...]`
- Return `Result<T, E>` from commands for proper error handling
- Use `Mutex<T>` for shared state accessed from multiple commands
- Add capabilities before using any plugin features
- Use `lib.rs` for shared code (required for mobile builds)

### Never Do

- Never use borrowed types ( `&str`) in async commands - use owned types
- Never block the main thread - use async for I/O operations
- Never hardcode paths - use Tauri path APIs ( `app.path()`)
- Never skip capability setup - even "safe" operations need permissions

### Common Mistakes

**Wrong - Borrowed type in async:**

```rust code-highlight
#[tauri::command]
async fn bad(name: &str) -> String { // Compile error!
    name.to_string()
}

```

**Correct - Owned type:**

```rust code-highlight
#[tauri::command]
async fn good(name: String) -> String {
    name
}

```

**Why:** Async commands cannot borrow data across await points; Tauri requires owned types for async command parameters.

## Known Issues Prevention

IssueRoot CauseSolution"Command not found"Missing from `generate_handler!`Add command to handler macro"Permission denied"Missing capabilityAdd to `capabilities/default.json`State panic on accessType mismatch in `State<T>`Use exact type from `.manage()`White screen on launchFrontend not buildingCheck `beforeDevCommand` in configIPC timeoutBlocking async commandRemove blocking code or use spawnMobile build failsMissing Rust targetsRun `rustup target add <target>`

## Configuration Reference

### tauri.conf.json

```json code-highlight
{
    "$schema": "./gen/schemas/desktop-schema.json",
    "productName": "my-app",
    "version": "1.0.0",
    "identifier": "com.example.myapp",
    "build": {
        "devUrl": "http://localhost:5173",
        "frontendDist": "../dist",
        "beforeDevCommand": "npm run dev",
        "beforeBuildCommand": "npm run build"
    },
    "app": {
        "windows": [{
            "label": "main",
            "title": "My App",
            "width": 800,
            "height": 600
        }],
        "security": {
            "csp": "default-src 'self'; img-src 'self' data:",
            "capabilities": ["default"]
        }
    },
    "bundle": {
        "active": true,
        "targets": "all",
        "icon": ["icons/icon.icns", "icons/icon.ico", "icons/icon.png"]
    }
}

```

**Key settings:**

- `build.devUrl`: Must match your frontend dev server port
- `app.security.capabilities`: Array of capability file identifiers

### Cargo.toml

```toml code-highlight
[package]
name = "app"
version = "0.1.0"
edition = "2021"

[lib]
name = "app_lib"
crate-type = ["staticlib", "cdylib", "rlib"]

[build-dependencies]
tauri-build = { version = "2", features = [] }

[dependencies]
tauri = { version = "2", features = [] }
serde = { version = "1", features = ["derive"] }
serde_json = "1"

```

**Key settings:**

- `[lib]` section: Required for mobile builds
- `crate-type`: Must include all three types for cross-platform

## Common Patterns

### Error Handling Pattern

```rust code-highlight
use thiserror::Error;

#[derive(Debug, Error)]
enum AppError {
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
    #[error("Not found: {0}")]
    NotFound(String),
}

impl serde::Serialize for AppError {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where S: serde::ser::Serializer {
        serializer.serialize_str(self.to_string().as_ref())
    }
}

#[tauri::command]
fn risky_operation() -> Result<String, AppError> {
    Ok("success".into())
}

```

### State Management Pattern

```rust code-highlight
use std::sync::Mutex;
use tauri::State;

struct AppState {
    counter: u32,
}

#[tauri::command]
fn increment(state: State<'_, Mutex<AppState>>) -> u32 {
    let mut s = state.lock().unwrap();
    s.counter += 1;
    s.counter
}

// In builder:
tauri::Builder::default()
    .manage(Mutex::new(AppState { counter: 0 }))

```

### Event Emission Pattern

```rust code-highlight
use tauri::Emitter;

#[tauri::command]
fn start_task(app: tauri::AppHandle) {
    std::thread::spawn(move || {
        app.emit("task-progress", 50).unwrap();
        app.emit("task-complete", "done").unwrap();
    });
}

```

```typescript code-highlight
import { listen } from '@tauri-apps/api/event';

const unlisten = await listen('task-progress', (e) => {
    console.log('Progress:', e.payload);
});
// Call unlisten() when done

```

### Channel Streaming Pattern

```rust code-highlight
use tauri::ipc::Channel;

#[derive(Clone, serde::Serialize)]
#[serde(tag = "event", content = "data")]
enum DownloadEvent {
    Progress { percent: u32 },
    Complete { path: String },
}

#[tauri::command]
async fn download(url: String, on_event: Channel<DownloadEvent>) {
    for i in 0..=100 {
        on_event.send(DownloadEvent::Progress { percent: i }).unwrap();
    }
    on_event.send(DownloadEvent::Complete { path: "/downloads/file".into() }).unwrap();
}

```

```typescript code-highlight
import { invoke, Channel } from '@tauri-apps/api/core';

const channel = new Channel<DownloadEvent>();
channel.onmessage = (msg) => console.log(msg.event, msg.data);
await invoke('download', { url: 'https://...', onEvent: channel });

```

## Bundled Resources

### References

Located in `references/`:

- [`capabilities-reference.md`](https://github.com/nodnarbnitram/claude-code-extensions/blob/HEAD/.claude/skills/tauri-v2/references/capabilities-reference.md) \- Permission patterns and examples
- [`ipc-patterns.md`](https://github.com/nodnarbnitram/claude-code-extensions/blob/HEAD/.claude/skills/tauri-v2/references/ipc-patterns.md) \- Complete IPC examples

> **Note:** For deep dives on specific topics, see the reference files above.

## Dependencies

### Required

PackageVersionPurpose`@tauri-apps/cli`^2.0.0CLI tooling`@tauri-apps/api`^2.0.0Frontend APIs`tauri`^2.0.0Rust core`tauri-build`^2.0.0Build scripts

### Optional (Plugins)

PackageVersionPurpose`tauri-plugin-fs`^2.0.0File system access`tauri-plugin-dialog`^2.0.0Native dialogs`tauri-plugin-shell`^2.0.0Shell commands, open URLs`tauri-plugin-http`^2.0.0HTTP client`tauri-plugin-store`^2.0.0Key-value storage

## Official Documentation

- [Tauri v2 Documentation](https://v2.tauri.app/)
- [Commands Reference](https://v2.tauri.app/develop/calling-rust/)
- [Capabilities & Permissions](https://v2.tauri.app/security/capabilities/)
- [Configuration Reference](https://v2.tauri.app/reference/config/)

## Troubleshooting

### White Screen on Launch

**Symptoms:** App launches but shows blank white screen

**Solution:**

1. Verify `devUrl` matches your frontend dev server port
2. Check `beforeDevCommand` runs your dev server
3. Open DevTools (Cmd+Option+I / Ctrl+Shift+I) to check for errors

### Command Returns Undefined

**Symptoms:** `invoke()` returns undefined instead of expected value

**Solution:**

1. Verify command is in `generate_handler![]`
2. Check Rust command actually returns a value
3. Ensure argument names match (camelCase in JS, snake\_case in Rust by default)

### Mobile Build Failures

**Symptoms:** Android/iOS build fails with missing target

**Solution:**

```bash code-highlight
# Android targets
rustup target add aarch64-linux-android armv7-linux-androideabi i686-linux-android x86_64-linux-android

# iOS targets (macOS only)
rustup target add aarch64-apple-ios x86_64-apple-ios aarch64-apple-ios-sim

```

## Setup Checklist

Before using this skill, verify:

- `npx tauri info` shows correct Tauri v2 versions
- `src-tauri/capabilities/default.json` exists with at least `core:default`
- All commands registered in `generate_handler![]`
- `lib.rs` contains shared code (for mobile support)
- Required Rust targets installed for target platforms

Weekly Installs

2.8K

Repository

[nodnarbnitram/c…tensions](https://github.com/nodnarbnitram/claude-code-extensions "nodnarbnitram/claude-code-extensions")

GitHub Stars

5

First Seen

Jan 24, 2026

Security Audits

[Gen Agent Trust HubPass](/nodnarbnitram/claude-code-extensions/tauri-v2/security/agent-trust-hub) [SocketPass](/nodnarbnitram/claude-code-extensions/tauri-v2/security/socket) [SnykWarn](/nodnarbnitram/claude-code-extensions/tauri-v2/security/snyk)

Installed on

opencode2.4K

codex2.4K

gemini-cli2.4K

github-copilot2.3K

kimi-cli2.2K

amp2.2K