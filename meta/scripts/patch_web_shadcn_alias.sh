#!/usr/bin/env bash
set -euo pipefail

WEB="client/web"
cd "$WEB"

echo "==> Using pnpm (recommended). If you used npm earlier, we'll clean that up."
# Make sure pnpm exists; if not, fallback to npm but warn.
if ! command -v pnpm >/dev/null 2>&1; then
  echo "WARN: pnpm not found. Install with: corepack enable && corepack prepare pnpm@latest --activate"
  echo "Continuing with npm… (not recommended)."
  export USE_NPM=1
else
  export USE_NPM=0
fi

pkg_runner() {
  if [ "$USE_NPM" = "1" ]; then npm "$@"; else pnpm "$@"; fi
}

echo "==> Ensure tsconfig.json has alias '@/ -> src/*' and baseUrl '.'"
# Patch tsconfig.json (idempotent)
node - <<'JS'
const fs = require('fs');
const p = 'tsconfig.json';
const j = JSON.parse(fs.readFileSync(p,'utf8'));
j.compilerOptions ||= {};
j.compilerOptions.baseUrl = '.';
j.compilerOptions.paths ||= {};
if (!j.compilerOptions.paths['@/*']?.includes('src/*')) {
  j.compilerOptions.paths['@/*'] = ['src/*'];
}
fs.writeFileSync(p, JSON.stringify(j, null, 2) + '\n');
console.log('tsconfig.json patched');
JS

echo "==> Patch vite.config.ts to add resolve.alias '@' → ./src"
# Works with ESM config
if grep -q "resolve:" vite.config.ts; then
  # Already has resolve; ensure alias '@'
  node - <<'JS'
const fs = require('fs');
let s = fs.readFileSync('vite.config.ts','utf8');
if (!s.includes("alias: { '@':")) {
  s = s.replace(
    /export default defineConfig\(\{\s*/m,
    `import { fileURLToPath, URL } from 'node:url'\n\nexport default defineConfig({\n`
  );
  s = s.replace(
    /server:\s*\{[^}]*\}(\s*,)?\s*\}/m,
    (m)=>m // leave as is
  );
  if (s.includes('resolve:')) {
    s = s.replace(/resolve:\s*\{[^}]*\}/m, (m)=>{
      if (m.includes("alias:")) {
        return m.replace(/alias:\s*\{/, `alias: { '@': fileURLToPath(new URL('./src', import.meta.url)), `);
      }
      return m.replace(/\{/, `{ alias: { '@': fileURLToPath(new URL('./src', import.meta.url)) }, `);
    });
  } else {
    s = s.replace(/plugins:\s*\[[^\]]*\](\s*,)?/m, (m)=>`${m},
  resolve: {
    alias: { '@': fileURLToPath(new URL('./src', import.meta.url)) }
  }`);
  }
  fs.writeFileSync('vite.config.ts', s);
  console.log('vite.config.ts patched (alias added)');
} else {
  // No resolve at all; inject one
  let s = fs.readFileSync('vite.config.ts','utf8');
  if (!s.includes("fileURLToPath(new URL('./src'"))) {
    s = `import { fileURLToPath, URL } from 'node:url'\n` + s;
  }
  s = s.replace(/export default defineConfig\(\{\s*/, `export default defineConfig({\n  resolve: { alias: { '@': fileURLToPath(new URL('./src', import.meta.url)) } },\n`);
  fs.writeFileSync('vite.config.ts', s);
  console.log('vite.config.ts patched (resolve block injected)');
}
JS
else
  # No resolve at all; simple inject
  sed -i '1i import { fileURLToPath, URL } from "node:url"' vite.config.ts
  awk 'BEGIN{p=1}/export default defineConfig\(\{/{print;print "  resolve: { alias: { '\''@'\'': fileURLToPath(new URL('\''./src'\'', import.meta.url)) } },";p=0;next}1' vite.config.ts > .vite.tmp && mv .vite.tmp vite.config.ts
  echo "vite.config.ts patched (simple inject)"
fi

echo "==> Remove duplicate Tailwind config (keep the TypeScript one)"
# Keep tailwind.config.ts; remove tailwind.config.js if empty boilerplate
if [ -f tailwind.config.js ]; then
  if grep -q 'content: \[\]' tailwind.config.js; then
    rm -f tailwind.config.js
    echo "Removed tailwind.config.js"
  else
    echo "NOTE: tailwind.config.js contains custom content. Prefer merging into tailwind.config.ts then delete .js."
  fi
fi

echo "==> Ensure Tailwind CSS file imports exist and include base tokens (optional but helpful for shadcn)"
mkdir -p src/styles
if ! grep -q '@tailwind base' src/styles/tailwind.css 2>/dev/null; then
  cat > src/styles/tailwind.css <<'CSS'
@tailwind base;
@tailwind components;
@tailwind utilities;

/* Optional: design tokens compatible with shadcn defaults */
@layer base {
  :root {
    --background: 0 0% 100%;
    --foreground: 222.2 84% 4.9%;
    --border: 214.3 31.8% 91.4%;
    --input: 214.3 31.8% 91.4%;
    --ring: 222.2 84% 4.9%;
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
    --card: 0 0% 100%;
    --card-foreground: 222.2 84% 4.9%;
    --radius: 0.5rem;
  }
}
CSS
  echo "Wrote src/styles/tailwind.css"
fi

echo "==> Ensure tailwind.config.ts scans the right folders"
# Make sure src/components is scanned (where shadcn puts files)
node - <<'JS'
const fs = require('fs');
const p = 'tailwind.config.ts';
let s = fs.readFileSync(p,'utf8');
if (!s.includes('./src/components')) {
  s = s.replace(/content:\s*\[/, `content: [\n    "./src/components/**/*.{ts,tsx}",`);
  fs.writeFileSync(p, s);
  console.log('tailwind.config.ts content globs updated (src/components)');
} else {
  console.log('tailwind.config.ts already scans src/components');
}
JS

echo "==> Install UI deps (tailwindcss-animate, cva, clsx, lucide, radix slot)"
if [ "$USE_NPM" = "1" ]; then
  npm i -D tailwindcss tailwindcss-animate postcss autoprefixer @vitejs/plugin-react class-variance-authority clsx lucide-react typescript vite @types/react @types/react-dom
  npm i @radix-ui/react-slot
else
  pnpm add -D tailwindcss tailwindcss-animate postcss autoprefixer @vitejs/plugin-react class-variance-authority clsx lucide-react typescript vite @types/react @types/react-dom
  pnpm add @radix-ui/react-slot
fi

echo "==> Run shadcn init non-interactively (should now pass alias check)"
npx shadcn@latest init -y || true

echo "==> Optionally add a couple of starter components (idempotent)"
npx shadcn@latest add button card || true

echo "==> Done. Try: pnpm dev"
