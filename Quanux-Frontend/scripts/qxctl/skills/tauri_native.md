tauri-native-api-integration by oimiragieo/agent-studio

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [oimiragieo](/oimiragieo)/ [agent-studio](/oimiragieo/agent-studio)/tauri-native-api-integration

# tauri-native-api-integration

`$ npx skills add https://github.com/oimiragieo/agent-studio --skill tauri-native-api-integration`

SKILL.md

# Tauri Native Api Integration Skill

- Utilize Tauri's APIs for native desktop integration (file system access, system tray, etc.).
- Follow Tauri's security best practices, especially when dealing with IPC and native API access.
- Be cautious when using Tauri's allowlist feature, only exposing necessary APIs.


## Iron Laws

1. **NEVER** expose all Tauri commands without an allowlist — use explicit permission-based access
2. **ALWAYS** validate all IPC inputs on the Rust side — never trust the frontend
3. **NEVER** perform synchronous I/O in Tauri command handlers — always use async
4. **ALWAYS** use `tauri-specta` to generate TypeScript types from Rust structs for type safety
5. **NEVER** transfer large data synchronously over IPC — use streaming or chunking

## Anti-Patterns

Anti-PatternWhy It FailsCorrect ApproachNo command allowlistAll Rust functions exposed to frontend; attack surface maximizedAllowlist only specific commands needed by the frontendTrusting frontend inputMalicious payloads can exploit Rust codeValidate and sanitize all IPC inputs on the Rust sideSynchronous I/O in commandsBlocks the Tauri event loop; UI freezesUse async Rust for all I/O operations in command handlersMissing TypeScript typesRuntime type mismatches between Rust and frontendUse tauri-specta to generate TypeScript types from Rust structsLarge synchronous data transfersIPC bottleneck causes UI stutteringStream or chunk large data; avoid transferring full datasets over IPC

## Memory Protocol (MANDATORY)

**Before starting:**

```bash code-highlight
cat .claude/context/memory/learnings.md

```

**After completing:** Record any new patterns or exceptions discovered.

> ASSUME INTERRUPTION: Your context may reset. If it's not in memory, it didn't happen.

Weekly Installs

64

Repository

[oimiragieo/agent-studio](https://github.com/oimiragieo/agent-studio "oimiragieo/agent-studio")

GitHub Stars

19

First Seen

Jan 27, 2026

Security Audits

[Gen Agent Trust HubPass](/oimiragieo/agent-studio/tauri-native-api-integration/security/agent-trust-hub) [SocketPass](/oimiragieo/agent-studio/tauri-native-api-integration/security/socket) [SnykFail](/oimiragieo/agent-studio/tauri-native-api-integration/security/snyk)

Installed on

github-copilot63

gemini-cli62

codex61

kimi-cli61

cursor61

opencode61