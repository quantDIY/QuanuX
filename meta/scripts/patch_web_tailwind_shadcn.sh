#!/usr/bin/env bash
set -euo pipefail

# repo root guard
test -f package.json -o -d server || { echo "Run from repo root (must see ./server)"; exit 1; }

WEB="client/web"
test -d "$WEB" || { echo "Missing $WEB. Run the frontends scaffold first."; exit 1; }

echo "==> Installing Vite React plugin, Tailwind toolchain, shadcn prereqs"
cd "$WEB"

# core dev deps
pnpm add -D @vitejs/plugin-react tailwindcss postcss autoprefixer

# shadcn + friends (runtime deps used by components)
pnpm add class-variance-authority clsx tailwind-merge lucide-react tailwindcss-animate

# initialize tailwind boilerplate (creates tailwind.config.js + postcss.config.js if absent)
npx --yes tailwindcss init -p

# --- Tailwind config (TS flavor, includes shadcn defaults) ---
cat > tailwind.config.ts <<'EOF'
import type { Config } from "tailwindcss"

export default {
  darkMode: ["class"],
  content: [
    "./index.html",
    "./src/**/*.{ts,tsx,jsx,js,mdx}"
  ],
  theme: {
    container: { center: true, padding: "2rem", screens: { "2xl": "1400px" } },
    extend: {
      colors: {
        border: "hsl(var(--border))",
        input: "hsl(var(--input))",
        ring: "hsl(var(--ring))",
        background: "hsl(var(--background))",
        foreground: "hsl(var(--foreground))",
        primary: { DEFAULT: "hsl(var(--primary))", foreground: "hsl(var(--primary-foreground))" },
        secondary: { DEFAULT: "hsl(var(--secondary))", foreground: "hsl(var(--secondary-foreground))" },
        destructive: { DEFAULT: "hsl(var(--destructive))", foreground: "hsl(var(--destructive-foreground))" },
        muted: { DEFAULT: "hsl(var(--muted))", foreground: "hsl(var(--muted-foreground))" },
        accent: { DEFAULT: "hsl(var(--accent))", foreground: "hsl(var(--accent-foreground))" },
        popover: { DEFAULT: "hsl(var(--popover))", foreground: "hsl(var(--popover-foreground))" },
        card: { DEFAULT: "hsl(var(--card))", foreground: "hsl(var(--card-foreground))" }
      },
      borderRadius: { lg: "var(--radius)", md: "calc(var(--radius) - 2px)", sm: "calc(var(--radius) - 4px)" },
      keyframes: {
        "accordion-down": { from: { height: "0" }, to: { height: "var(--radix-accordion-content-height)" } },
        "accordion-up": { from: { height: "var(--radix-accordion-content-height)" }, to: { height: "0" } }
      },
      animation: {
        "accordion-down": "accordion-down 0.2s ease-out",
        "accordion-up": "accordion-up 0.2s ease-out"
      }
    }
  },
  plugins: [require("tailwindcss-animate")],
} satisfies Config
EOF

# ensure vite picks up TS tailwind config
# (Vite will work with either tailwind.config.ts or .js; we keep ts)

# --- PostCSS config (overwrite to known-good minimal) ---
cat > postcss.config.js <<'EOF'
export default {
  plugins: {
    tailwindcss: {},
    autoprefixer: {},
  },
}
EOF

# --- Base CSS with tailwind directives + shadcn CSS vars ---
mkdir -p src
cat > src/index.css <<'EOF'
@tailwind base;
@tailwind components;
@tailwind utilities;

/* shadcn CSS variables (light/dark) */
@layer base {
  :root {
    --background: 0 0% 100%;
    --foreground: 222.2 84% 4.9%;
    --card: 0 0% 100%;
    --card-foreground: 222.2 84% 4.9%;
    --popover: 0 0% 100%;
    --popover-foreground: 222.2 84% 4.9%;
    --primary: 222.2 47.4% 11.2%;
    --primary-foreground: 210 40% 98%;
    --secondary: 210 40% 96.1%;
    --secondary-foreground: 222.2 47.4% 11.2%;
    --muted: 210 40% 96.1%;
    --muted-foreground: 215.4 16.3% 46.9%;
    --accent: 210 40% 96.1%;
    --accent-foreground: 222.2 47.4% 11.2%;
    --destructive: 0 84.2% 60.2%;
    --destructive-foreground: 210 40% 98%;
    --border: 214.3 31.8% 91.4%;
    --input: 214.3 31.8% 91.4%;
    --ring: 222.2 84% 4.9%;
    --radius: 0.75rem;
  }
  .dark {
    --background: 222.2 84% 4.9%;
    --foreground: 210 40% 98%;
    --card: 222.2 84% 4.9%;
    --card-foreground: 210 40% 98%;
    --popover: 222.2 84% 4.9%;
    --popover-foreground: 210 40% 98%;
    --primary: 210 40% 98%;
    --primary-foreground: 222.2 47.4% 11.2%;
    --secondary: 217.2 32.6% 17.5%;
    --secondary-foreground: 210 40% 98%;
    --muted: 217.2 32.6% 17.5%;
    --muted-foreground: 215 20.2% 65.1%;
    --accent: 217.2 32.6% 17.5%;
    --accent-foreground: 210 40% 98%;
    --destructive: 0 62.8% 30.6%;
    --destructive-foreground: 210 40% 98%;
    --border: 217.2 32.6% 17.5%;
    --input: 217.2 32.6% 17.5%;
    --ring: 212.7 26.8% 83.9%;
  }
  * { @apply border-border; }
  body { @apply bg-background text-foreground; }
}
EOF

# --- Vite config: ensure plugin-react is referenced ---
cat > vite.config.ts <<'EOF'
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  server: { port: 5173, host: true }
});
EOF

# --- minimal shadcn components (Button + Card) w/o interactive CLI ---
mkdir -p src/components/ui
cat > src/components/ui/button.tsx <<'EOF'
import * as React from "react"
import { Slot } from "@radix-ui/react-slot"
import { cva, type VariantProps } from "class-variance-authority"
import { cn } from "../utils"

const buttonVariants = cva(
  "inline-flex items-center justify-center whitespace-nowrap rounded-md text-sm font-medium transition-colors" +
  " focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring disabled:pointer-events-none disabled:opacity-50",
  {
    variants: {
      variant: {
        default: "bg-primary text-primary-foreground hover:opacity-90",
        outline: "border border-input bg-background hover:bg-accent hover:text-accent-foreground",
        ghost: "hover:bg-accent hover:text-accent-foreground",
        link: "text-primary underline-offset-4 hover:underline",
      },
      size: {
        default: "h-9 px-4 py-2",
        sm: "h-8 rounded-md px-3",
        lg: "h-10 rounded-md px-8",
        icon: "h-9 w-9",
      },
    },
    defaultVariants: { variant: "default", size: "default" },
  }
)

export interface ButtonProps extends
  React.ButtonHTMLAttributes<HTMLButtonElement>,
  VariantProps<typeof buttonVariants> {
  asChild?: boolean
}

export const Button = React.forwardRef<HTMLButtonElement, ButtonProps>(
  ({ className, variant, size, asChild = false, ...props }, ref) => {
    const Comp = asChild ? Slot : "button"
    return (
      <Comp
        className={cn(buttonVariants({ variant, size, className }))}
        ref={ref}
        {...props}
      />
    )
  }
)
Button.displayName = "Button"
EOF

cat > src/components/ui/card.tsx <<'EOF'
import * as React from "react"
import { cn } from "../utils"

export function Card({ className, ...props }: React.HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("rounded-xl border bg-card text-card-foreground shadow", className)} {...props} />
}
export function CardHeader({ className, ...props }: React.HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("flex flex-col space-y-1.5 p-6", className)} {...props} />
}
export function CardTitle({ className, ...props }: React.HTMLAttributes<HTMLHeadingElement>) {
  return <h3 className={cn("text-2xl font-semibold leading-none tracking-tight", className)} {...props} />
}
export function CardContent({ className, ...props }: React.HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("p-6 pt-0", className)} {...props} />
}
EOF

cat > src/components/utils.ts <<'EOF'
import { type ClassValue, clsx } from "clsx"
import { twMerge } from "tailwind-merge"
export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs))
}
EOF

# --- hook tailwind into the app ---
mkdir -p src/ui
cat > src/ui/App.tsx <<'EOF'
import React from "react";
import "../index.css";
import { Button } from "../components/ui/button";
import { Card, CardHeader, CardTitle, CardContent } from "../components/ui/card";
import { Health } from "./Health";

export const App: React.FC = () => {
  const [dark, setDark] = React.useState(false);
  React.useEffect(() => {
    document.documentElement.classList.toggle("dark", dark);
  }, [dark]);

  return (
    <div className="min-h-screen p-6">
      <div className="flex items-center justify-between mb-6">
        <h1 className="text-3xl font-bold">QuanuX Web</h1>
        <Button variant="outline" onClick={() => setDark(d => !d)}>
          Toggle {dark ? "Light" : "Dark"}
        </Button>
      </div>

      <Card>
        <CardHeader>
          <CardTitle>Backend / Bridge Health</CardTitle>
        </CardHeader>
        <CardContent>
          <Health />
        </CardContent>
      </Card>
    </div>
  );
};
EOF

# --- Health component (keep same logic, add tailwind polish) ---
cat > src/ui/Health.tsx <<'EOF'
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
EOF

# --- Minimal MCP stub (placeholder wiring) ---
mkdir -p src/lib/mcp
cat > src/lib/mcp/client.ts <<'EOF'
/**
 * Minimal MCP client stub (placeholder).
 * In the web app we keep a typed interface and wire an adapter later.
 */
export type MCPToolCall = { name: string; args?: Record<string, unknown> }
export type MCPResult = { ok: boolean; data?: unknown; error?: string }

export interface MCPAdapter {
  call(tool: MCPToolCall): Promise<MCPResult>
}

class NoopAdapter implements MCPAdapter {
  async call(_tool: MCPToolCall): Promise<MCPResult> {
    return { ok: false, error: "No MCP adapter configured" }
  }
}

let adapter: MCPAdapter = new NoopAdapter()
export function setMCPAdapter(a: MCPAdapter) { adapter = a }
export async function mcpCall(t: MCPToolCall) { return adapter.call(t) }
EOF

echo "==> Done. Now install node_modules and run dev:"
echo "cd $WEB && pnpm install && cp -n .env.example .env || true && pnpm dev"
