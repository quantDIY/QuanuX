let _manualBase: string | null = null;

function isBrowser(): boolean {
  return typeof window !== "undefined";
}
function isTauri(): boolean {
  // Vite-in-Tauri still has import.meta.env, but we can signal via a sentinel on window.
  return isBrowser() && ("__TAURI_IPC__" in (window as any));
}

export function setApiBase(url: string | null) {
  _manualBase = url || null;
  if (isBrowser() && url) {
    try { localStorage.setItem("QUANUX_API_BASE", url); } catch {}
  }
}

export function getApiBase(): string {
  // 1) manual override (setApiBase)
  if (_manualBase) return _manualBase;

  // 2) global window override (lets you inject at runtime)
  if (isBrowser() && (window as any).QUANUX_API_BASE) {
    return (window as any).QUANUX_API_BASE as string;
  }

  // 3) persisted setting
  if (isBrowser()) {
    try {
      const stored = localStorage.getItem("QUANUX_API_BASE");
      if (stored) return stored;
    } catch {}
  }

  // 4) env (works in Vite + Tauri builds)
  const envBase = (import.meta as any).env?.VITE_API_BASE;
  if (typeof envBase === "string" && envBase.trim().length > 0) return envBase;

  // 5) auto-defaults
  if (isTauri()) return "http://127.0.0.1:8077";
  return "/api";
}
