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
      "@quanux/shared-ui": fileURLToPath(new URL("../../shared", import.meta.url)),
    },
  },
  server: {
    port: 1420,         // <- desktop on 1420 (Tauri default)
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

