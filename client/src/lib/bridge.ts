import { getApiBase } from "./config";

export type HealthResponse = { status: string; version?: string; detail?: unknown };

export async function health(): Promise<HealthResponse> {
  const base = getApiBase().replace(/\/+$/, "");
  const url = `${base}/api/health`;
  const res = await fetch(url, { headers: { Accept: "application/json" } });
  if (!res.ok) return { status: `error:${res.status}` };
  try { return await res.json(); } catch { return { status: "ok" }; }
}
