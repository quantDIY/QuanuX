// SAMPLE ONLY — demonstrate how the React layer would call MCP tools.
// Desktop will rely on Tauri IPC to get the token (never in browser storage).

import { createFigmaSdk } from "../../../integrations/mcp/figma/sdk";

export function useFigmaMcp() {
  async function getToken(): Promise<string | null> {
    // In Tauri desktop:
    //   return await window.__TAURI__.invoke("keychain_get", { key: "FIGMA_PERSONAL_ACCESS_TOKEN" });
    return null; // stub
  }

  const sdk = createFigmaSdk(getToken);

  return {
    isAvailable: false, // becomes true on desktop builds
    sdk,
  };
}
