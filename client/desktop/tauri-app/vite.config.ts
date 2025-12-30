import path from "node:path";
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { fileURLToPath, URL } from "node:url";

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
      "@/shared": fileURLToPath(new URL("../shared", import.meta.url)),
      "@quanux/shared": fileURLToPath(new URL("../../shared", import.meta.url)),
    },
  },
  server: {
    port: 5174,         // <- desktop on 5174
    strictPort: true,   // <- never auto-switch
    proxy: {
      "/api": {
        target: "http://127.0.0.1:8000",
        changeOrigin: true,
        ws: true,
      },
    },
  },
});

