let API_BASE =
  (typeof window !== "undefined" && (window as any).__QUANUX_API_BASE__) ||
  "http://localhost:8000";

export function getApiBase() {
  return API_BASE;
}

export function setApiBase(url: string) {
  API_BASE = url;
  if (typeof window !== "undefined") {
    (window as any).__QUANUX_API_BASE__ = url;
  }
}
