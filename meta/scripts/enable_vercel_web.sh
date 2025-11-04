#!/usr/bin/env bash
set -euo pipefail

# Run from repo root
test -d server || { echo "Run from repo root (must see ./server)"; exit 1; }

WEB="client/web"
test -d "$WEB" || { echo "Missing $WEB — run the web scaffold first."; exit 1; }

# 1) vercel.json — zero-config build for Vite + Edge runtime enabled
cat >"$WEB/vercel.json"<<'EOF'
{
  "$schema": "https://openapi.vercel.sh/vercel.json",
  "buildCommand": "pnpm build",
  "outputDirectory": "dist",
  "framework": "vite",
  "cleanUrls": true,
  "trailingSlash": false,
  "functions": {
    "api/**": {
      "runtime": "edge",
      "memory": 128
    }
  }
}
EOF

# 2) Edge middleware — attach common headers, enforce /api path handling
cat >"$WEB/middleware.ts"<<'EOF'
import type { NextRequest } from "next/server";

export const config = {
  matcher: ["/api/:path*"]
};

export default async function middleware(_req: NextRequest) {
  // We let the actual API route files handle proxying logic.
  // This file exists to demonstrate where you'd add cross-cutting concerns.
  return new Response(null, { status: 200, headers: { "x-quanux": "edge-middleware" }});
}
EOF

# 3) Generic proxy function for /api/* -> ${BACKEND_URL}/*
mkdir -p "$WEB/api"

cat >"$WEB/api/[...path].ts"<<'EOF'
export const config = {
  runtime: "edge"
};

const allowHeaders = [
  "authorization",
  "content-type",
  "x-requested-with",
  "accept",
  "origin"
].join(", ");

function makeCORS(origin: string | null) {
  return {
    "access-control-allow-origin": origin ?? "*",
    "access-control-allow-headers": allowHeaders,
    "access-control-allow-methods": "GET,POST,PUT,PATCH,DELETE,OPTIONS",
    "access-control-allow-credentials": "true"
  };
}

export default async function handler(req: Request) {
  // IMPORTANT: Set this in Vercel project settings (Environment Variables)
  // e.g., BACKEND_URL=https://your-self-hosted-quanux.example.com
  const upstream = process.env.BACKEND_URL;
  if (!upstream) {
    return new Response(
      JSON.stringify({ ok: false, error: "Missing BACKEND_URL env" }),
      { status: 500, headers: { "content-type": "application/json" } }
    );
  }

  const url = new URL(req.url);
  const rest = url.pathname.replace(/^\/api\/?/, "");
  const target = new URL(rest, upstream.endsWith("/") ? upstream : upstream + "/");

  // Preflight
  if (req.method.toUpperCase() === "OPTIONS") {
    return new Response(null, { status: 204, headers: makeCORS(req.headers.get("origin")) });
  }

  const headers = new Headers(req.headers);
  // Remove any host header that could confuse upstream
  headers.delete("host");

  const proxied = await fetch(target.toString(), {
    method: req.method,
    headers,
    body: ["GET", "HEAD"].includes(req.method.toUpperCase()) ? undefined : req.body
  });

  // Pass through body, but fix CORS for the browser
  const resHeaders = new Headers(proxied.headers);
  for (const [k] of resHeaders) {
    // Optionally strip hop-by-hop headers
    if (/^connection$/i.test(k)) resHeaders.delete(k);
  }
  const origin = req.headers.get("origin");
  const cors = makeCORS(origin);
  Object.entries(cors).forEach(([k, v]) => resHeaders.set(k, v));

  return new Response(proxied.body, {
    status: proxied.status,
    headers: resHeaders
  });
}
EOF

# 4) Explicit health endpoint (/api/health -> ${BACKEND_URL}/health)
cat >"$WEB/api/health.ts"<<'EOF'
export const config = { runtime: "edge" };

export default async function handler(_req: Request) {
  const upstream = process.env.BACKEND_URL;
  if (!upstream) {
    return new Response(JSON.stringify({ ok: false, error: "Missing BACKEND_URL env" }), {
      status: 500,
      headers: { "content-type": "application/json" }
    });
  }
  const url = new URL("health", upstream.endsWith("/") ? upstream : upstream + "/");
  const r = await fetch(url.toString(), { headers: { "accept": "application/json" } });
  const body = await r.text();
  return new Response(body, {
    status: r.status,
    headers: {
      "content-type": r.headers.get("content-type") ?? "application/json",
      "access-control-allow-origin": "*"
    }
  });
}
EOF

# 5) .env.example — note: Vite client-side vars still use VITE_*; serverless uses BACKEND_URL
grep -q '^# Vercel / Edge' "$WEB/.env.example" 2>/dev/null || cat >>"$WEB/.env.example"<<'EOF'

# Vercel / Edge (server-side) — set in Vercel dashboard
# BACKEND_URL=https://your-self-hosted-quanux.example.com

# Client-side (exposed via Vite)
# VITE_BRIDGE_HEALTH=/api/health
EOF

# 6) Point the web Health component to the proxy by default (safe for local + vercel)
# Only patch if file exists and doesn't already use /api/health
if [ -f "$WEB/src/ui/Health.tsx" ]; then
  sed -i 's|VITE_BRIDGE_HEALTH") ?? "http://127.0.0.1:8077/health"|VITE_BRIDGE_HEALTH") ?? "/api/health"|' "$WEB/src/ui/Health.tsx" || true
fi

# 7) README note
if [ -f "$WEB/README.md" ]; then
  cat >>"$WEB/README.md"<<'EOF'

## Deploying to Vercel

1. In the Vercel project → **Settings → Environment Variables**, add:

   - `BACKEND_URL` = your self-hosted QuanuX bridge base URL (e.g. `https://bridge.example.com/`)

2. (Optional) In `.env` locally, set:
VITE_BRIDGE_HEALTH=/api/health


3. Deploy:
- `vercel` (with the Vercel CLI), or connect the repo and let Vercel auto-build.

**How it works:**  
- `client/web/api/**` are **Edge Functions** that proxy requests to `BACKEND_URL`.  
- Your browser calls `/api/*` on the Vercel domain; the function securely forwards to your self-hosted backend (CORS handled).
- The UI’s health panel hits `/api/health`, which maps to `${BACKEND_URL}/health`.
EOF
fi

echo "Vercel integration written to $WEB:"
echo " - vercel.json"
echo " - middleware.ts"
echo " - api/[...path].ts"
echo " - api/health.ts"
echo " - .env.example updated"
echo "Update Vercel env: BACKEND_URL=https://your-self-hosted-quanux.example.com"

