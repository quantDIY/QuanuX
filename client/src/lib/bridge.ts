// Minimal, robust joiner for API paths.
// Assumes getApiBase() returns "", "/", "/api", "http://host:port", etc.
// We ensure we end up with exactly one slash between base and path.

import { getApiBase } from "./config";

// If base = "/api", DO NOT prefix another "/api" here
export async function health() {
  const base = getApiBase(); // "/api" in dev via index.html or config default
  const res = await fetch(`${base}/health`);
  if (!res.ok) throw new Error(`Health ${res.status}`);
  return res.json();
}
