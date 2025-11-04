import type { NextRequest } from "next/server";

export const config = {
  matcher: ["/api/:path*"]
};

export default async function middleware(_req: NextRequest) {
  // We let the actual API route files handle proxying logic.
  // This file exists to demonstrate where you'd add cross-cutting concerns.
  return new Response(null, { status: 200, headers: { "x-quanux": "edge-middleware" }});
}
