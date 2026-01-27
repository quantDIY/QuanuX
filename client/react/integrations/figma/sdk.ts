// SAMPLE ONLY — replace during wiring.
// Intention: thin, typed wrapper the React code can import.
// Token never read here directly; retrieved via Tauri command.

export type FigmaFile = { name: string; key: string };

export interface FigmaSdk {
  listTeamProjects(teamId: string): Promise<unknown>;
  getFile(fileKey: string): Promise<unknown>;
}

export function createFigmaSdk(getToken: () => Promise<string | null>): FigmaSdk {
  const base = "https://api.figma.com/v1";

  async function headers() {
    const token = await getToken();
    if (!token) throw new Error("Figma token not available");
    return { Authorization: `Bearer ${token}` };
  }

  return {
    async listTeamProjects(teamId: string) {
      // placeholder only — do not ship before wiring
      const h = await headers();
      // return fetch(`${base}/teams/${teamId}/projects`, { headers: h });
      return { ok: false, note: "stub" };
    },
    async getFile(fileKey: string) {
      const h = await headers();
      // return fetch(`${base}/files/${fileKey}`, { headers: h });
      return { ok: false, note: "stub" };
    },
  };
}
