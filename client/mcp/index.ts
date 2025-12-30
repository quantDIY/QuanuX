
/**
 * QuanuX Shared MCP Client
 * Protocol-agnostic definitions for connecting to the Tool Authority.
 */

export type MCPToolCall = {
    name: string;
    args?: Record<string, unknown>;
};

export type MCPResult<T = unknown> = {
    ok: boolean;
    data?: T;
    error?: string;
};

export interface MCPTransport {
    call(tool: MCPToolCall): Promise<MCPResult>;
}

// Registry-aware types (could be code-generated from registry.yaml later)
export type ToolName =
    | 'repo.search'
    | 'repo.open'
    | 'build.web'
    | 'build.desktop'
    | 'test.server'
    | 'duckdb.query_readonly';

export class MCPClient {
    constructor(private transport: MCPTransport) { }

    async call<T>(name: ToolName, args?: Record<string, unknown>): Promise<MCPResult<T>> {
        return (await this.transport.call({ name, args })) as MCPResult<T>;
    }
}
