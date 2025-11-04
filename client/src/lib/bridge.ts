import { getApiBase } from "./config";

export async function fetchJSON<T=unknown>(path: string, init?: RequestInit): Promise<T> {
  const base = getApiBase();
  const url = path.startsWith("http") ? path : `${base.replace(/\/+$/, "")}/${path.replace(/^\/+/, "")}`;
  const res = await fetch(url, {
    headers: { "accept": "application/json", ...(init?.headers || {}) },
    ...init
  });
  if (!res.ok) {
    const text = await res.text().catch(() => "");
    throw new Error(`HTTP ${res.status} on ${url}${text ? ` — ${text}` : ""}`);
  }
  const ct = res.headers.get("content-type") || "";
  return (ct.includes("application/json") ? res.json() : (res.text() as any)) as Promise<T>;
}

export async function health(): Promise<any> {
  // convention: health lives at /health on the API base
  return fetchJSON("health");
}
