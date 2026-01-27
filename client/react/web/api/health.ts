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
