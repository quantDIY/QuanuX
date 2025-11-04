import React from "react";

export const Health: React.FC = () => {
  const [status, setStatus] = React.useState<null | { ok: boolean; details?: any }>(null);
  const [err, setErr] = React.useState<string | null>(null);
  const url = (import.meta as any).env?.VITE_BRIDGE_HEALTH ?? "http://127.0.0.1:8077/health";

  const ping = React.useCallback(async () => {
    try {
      setErr(null);
      const res = await fetch(url, { headers: { "Accept": "application/json" } });
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const json = await res.json();
      setStatus({ ok: true, details: json });
    } catch (e: any) {
      setStatus({ ok: false });
      setErr(e?.message || "Request failed");
    }
  }, [url]);

  React.useEffect(() => { void ping(); }, [ping]);

  return (
    <div>
      {status === null && <div className="text-muted-foreground">Checking…</div>}
      {status && status.ok && (
        <pre className="text-sm bg-muted/40 p-3 rounded-md overflow-x-auto">
          {JSON.stringify(status.details, null, 2)}
        </pre>
      )}
      {status && !status.ok && (
        <div className="text-sm text-red-600">Unavailable {err ? `— ${err}` : ""}</div>
      )}
      <button
        onClick={ping}
        className="mt-3 inline-flex items-center rounded-md border px-3 py-1 text-sm hover:bg-accent hover:text-accent-foreground"
      >
        Recheck
      </button>
    </div>
  );
};
