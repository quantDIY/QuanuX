import http from "node:http";

const server = http.createServer((req, res) => {
  if (req.url === "/api/health") {
    res.writeHead(200, { "Content-Type": "application/json" });
    res.end(JSON.stringify({
      ok: true,
      service: "mock-api",
      time: new Date().toISOString()
    }));
    return;
  }
  res.writeHead(404);
  res.end();
});

server.listen(8000, "0.0.0.0", () => {
  console.log("Mock API listening on http://0.0.0.0:8000  (GET /api/health)");
});
