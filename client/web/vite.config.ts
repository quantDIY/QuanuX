import path from "node:path";
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { fileURLToPath, URL } from "node:url";

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
      // Mirror desktop: expose client/src as @quanux/shared
      "@quanux/shared": path.resolve(__dirname, "../src"),
    },
  },
  server: {
    port: 5173,        // web on 5173
    strictPort: true,  // don't silently switch
    proxy: {
      "/api": {
        target: "http://127.0.0.1:8000",
        changeOrigin: true,
      },
    },
  },
});

