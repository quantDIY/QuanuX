import React from "react";
import { Button } from "../components/ui/button";
import { Card, CardHeader, CardTitle, CardContent } from "../components/ui/card";

// Use the shared config + bridge just like desktop:
import { getApiBase, setApiBase } from "@quanux/shared/lib/config";
import { health } from "@quanux/shared/lib/bridge";

export const App: React.FC = () => {
  const [dark, setDark] = React.useState(false);
  const [base, setBaseState] = React.useState<string>(getApiBase());
  const [msg, setMsg] = React.useState<string>("");

  React.useEffect(() => {
    document.documentElement.classList.toggle("dark", dark);
  }, [dark]);

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
      <div className="flex items-center justify-between mb-6">
        <h1 className="text-3xl font-bold">QuanuX Web</h1>
        <Button variant="outline" onClick={() => setDark((d) => !d)}>
          Toggle {dark ? "Light" : "Dark"}
        </Button>
      </div>

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
            placeholder="http://127.0.0.1:8000 or https://your-remote-host"
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
