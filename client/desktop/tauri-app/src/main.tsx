import React from "react";
import { createRoot } from "react-dom/client";
import { getApiBase, setApiBase } from "@quanux/shared/lib/config";
import { health } from "@quanux/shared/lib/bridge";

const App = () => {
  const [base, setBase] = React.useState<string>(getApiBase());
  const [msg, setMsg] = React.useState<string>("");

  const probe = async () => {
    try {
      const h = await health();
      setMsg(`Health OK: ${JSON.stringify(h)}`);
    } catch (e: any) {
      setMsg(`Health error: ${e?.message || "failed"}`);
    }
  };

  return (
    <div style={{ padding: 16, fontFamily: "Inter, system-ui, Arial" }}>
      <h1>QuanuX Desktop (Tauri)</h1>
      <p>Current API base: <code>{base}</code></p>
      <div style={{ display: "flex", gap: 8 }}>
        <input
          style={{ padding: 6, minWidth: 380 }}
          value={base}
          onChange={(e) => setBase(e.target.value)}
          placeholder="http://127.0.0.1:8077 or https://your-remote-host"
        />
        <button
          onClick={() => { setApiBase(base); setMsg("Saved new base URL"); }}
          style={{ padding: "6px 10px", cursor: "pointer" }}
        >
          Save
        </button>
        <button onClick={probe} style={{ padding: "6px 10px", cursor: "pointer" }}>Probe health</button>
      </div>
      <div style={{ marginTop: 10 }}>{msg}</div>
    </div>
  );
};

createRoot(document.getElementById("root")!).render(<App />);
