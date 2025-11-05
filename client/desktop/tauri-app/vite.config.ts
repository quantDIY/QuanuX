import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { fileURLToPath, URL } from "node:url";
import path from "node:path";

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
      // Monorepo shared code lives at client/src
      "@quanux/shared": path.resolve(__dirname, "../../src"),
      // optional legacy alias if you used it anywhere
      "@/shared": path.resolve(__dirname, "../shared"),
    },
  },
});
