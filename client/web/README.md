# QuanuX Web (React + Vite)
- `pnpm i` (or `npm i`, `yarn`)
- copy `.env.example` → `.env` and set `VITE_BRIDGE_HEALTH`
- `pnpm dev` then open the local dev URL

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
