---
name: native_ai_embedded
description: "Guide for connecting Local/Embedded AI Models (Ollama, Llama.cpp) to QuanuX C++ Engine."
---

# Native AI Connector (Embedded/Local)

The **Native AI Connector** allows QuanuX Execution Nodes to communicate directly with **Embedded AI Models** running on the local machine or private network. This bypasses slow/expensive Cloud APIs and enables high-frequency, privacy-preserving I.Q.

## 1. Supported Local Runners
QuanuX supports any runner that provides an OpenAI-compatible API (standard in 2024+).

### A. Ollama (Recommended)
1.  **Install**: [ollama.com](https://ollama.com)
2.  **Pull Model**: `ollama pull llama3`
3.  **Run**: `ollama serve` (Runs on port 11434 by default)
4.  **QuanuX Config**: 
    - No config needed! QuanuX will **Auto-Discover** Ollama on port 11434.

### B. Llama.cpp Server
1.  **Build**: `make server` in llama.cpp repo.
2.  **Run**: `./server -m my-model.gguf --port 8080 --host 0.0.0.0`
3.  **QuanuX Config**:
    - Auto-Discovers on port 8080.
    - Or force via env: `export QUANUX_AI_ENDPOINT="http://localhost:8080"`

### C. LM Studio / Text-Gen-WebUI
1.  Start the Local Server feature.
2.  Ensure "OpenAI Compatibility" is checked.

## 2. Configuration (`.env`)

| Variable | Description | Default |
| :--- | :--- | :--- |
| `QUANUX_AI_ENDPOINT` | URL of the AI Server. Leave empty for **Auto-Discovery**. | `AUTODETECT` |
| `QUANUX_AI_MODEL` | Model name to request (e.g., `llama3`, `mistral`). | `llama3` |
| `QUANUX_AI_PROVIDER` | Protocol dialect: `openai`, `ollama`, `anthropic`. | `openai` |
| `QUANUX_AI_KEY` | Optional API key (if server requires it). | `""` |

## 3. Usage in C++ Strategies

Strategies access the AI via the `OrderService` pointer passed during `on_init`.

```cpp
#include "quanux/common/StrategyInterface.h"
#include <cstring>
#include <iostream>

// In your strategy logic
void on_market_data(StrategyContext *ctx, const MarketUpdate *update) {
    if (update->price > 5000) {
        char response[1024];
        if (ctx->service->query_ai(ctx->service->engine_ctx, 
            "Market is over 5000. Buy or Sell?", 
            response, 1024)) {
            
            std::cout << "AI Advice: " << response << std::endl;
        }
    }
}
```

## 4. Advanced: Non-OpenAI Models
We built the bridge to be **Model Agnostic**.
If you are using a specialized local server (e.g. for Gemini Nano or unreleased models) that uses a different JSON format:
1.  Set `QUANUX_AI_PROVIDER=custom` (Future work) or `gemini_local`.
2.  The Bridge will adapt the payload structure automatically.

## 5. Port Forwarding (Remote Nodes)
If your AI beast machine is separate from your Trading Node:
1.  **SSH Tunnel**: `ssh -L 8080:localhost:8080 user@ai-server`
2.  **QuanuX**: Connects to `localhost:8080` as if it were local.
