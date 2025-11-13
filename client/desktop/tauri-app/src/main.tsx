import "./styles/tailwind.css";
import React from "react";
import { createRoot } from "react-dom/client";
import { getApiBase, setApiBase } from "@quanux/shared/lib/config";
import { health } from "@quanux/shared/lib/bridge";

const App = () => {
  const [base, setBaseState] = React.useState<string>(getApiBase());
  const [msg, setMsg] = React.useState<string>("");

  const saveBase = () => {
    setApiBase(base);
    setMsg("Saved new base URL");
  };

  const probe = async () => {
    try {
      const h = await health();
      setMsg(`Health OK: ${JSON.stringify(h)}`);
    } catch (e: any) {
      setMsg(`Health error: ${e?.message || "failed"}`);
    }
  };

  return (
    <div className="min-h-screen p-6 bg-background text-foreground">
      <h1 className="text-3xl font-bold mb-6">QuanuX Desktop (Tauri)</h1>

      <div className="rounded-xl border bg-card text-card-foreground shadow p-6 space-y-4">
        <div>
          <p className="text-sm text-muted-foreground">Current API base:</p>
          <code className="text-sm">{base}</code>
        </div>

        <div className="flex gap-2">
          <input
            className="min-w-[380px] px-3 py-2 rounded-md border border-input bg-background text-foreground"
            value={base}
            onChange={(e) => setBaseState(e.target.value)}
            placeholder="http://127.0.0.1:8077 or https://your-remote-host"
          />
          <button
            onClick={saveBase}
            className="inline-flex items-center rounded-md border px-3 py-2 text-sm hover:bg-accent hover:text-accent-foreground"
          >
            Save
          </button>
          <button
            onClick={probe}
            className="inline-flex items-center rounded-md border px-3 py-2 text-sm hover:bg-accent hover:text-accent-foreground"
          >
            Probe health
          </button>
        </div>

        <div className="text-sm">{msg}</div>
      </div>
    </div>
  );
};

createRoot(document.getElementById("root")!).render(<App />);
