// SAMPLE ONLY — registers MCP tools for agents inside the client.
// In wiring, we’ll add proper MCP transport (stdio/ws) via a Tauri plugin or Rust helper.

export const FigmaMcpTools = {
  "figma.list_files": async (args: { team_id: string }) => {
    return { ok: false, note: "stub", args };
  },
  "figma.get_file": async (args: { file_key: string }) => {
    return { ok: false, note: "stub", args };
  },
  "figma.get_nodes": async (args: { file_key: string; ids: string }) => {
    return { ok: false, note: "stub", args };
  },
};
