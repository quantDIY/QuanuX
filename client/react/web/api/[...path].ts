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
