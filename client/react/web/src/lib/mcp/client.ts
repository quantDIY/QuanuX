/**
 * Minimal MCP client stub (placeholder).
 * In the web app we keep a typed interface and wire an adapter later.
 */
export type MCPToolCall = { name: string; args?: Record<string, unknown> }
export type MCPResult = { ok: boolean; data?: unknown; error?: string }

export interface MCPAdapter {
  call(tool: MCPToolCall): Promise<MCPResult>
}

class NoopAdapter implements MCPAdapter {
  async call(_tool: MCPToolCall): Promise<MCPResult> {
    return { ok: false, error: "No MCP adapter configured" }
  }
}

let adapter: MCPAdapter = new NoopAdapter()
export function setMCPAdapter(a: MCPAdapter) { adapter = a }
export async function mcpCall(t: MCPToolCall) { return adapter.call(t) }
