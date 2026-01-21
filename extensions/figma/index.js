#!/usr/bin/env node

import { spawn } from 'child_process';
import path from 'path';
import { fileURLToPath } from 'url';
import fs from 'fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

// 1. Locate Binary
// In a pnpm workspace or flattened install, .bin is usually reliable.
const binPath = path.resolve(__dirname, 'node_modules', '.bin', 'figma-developer-mcp');

if (!fs.existsSync(binPath)) {
    console.error(`Error: Native binary not found at ${binPath}`);
    console.error("Please run 'pnpm install' in this directory.");
    process.exit(1);
}

// 2. Connector Logic
// We act as a stdio pass-through.
// Map QuanuX secret (QUANUX_figma) to Official ENV (FIGMA_API_KEY)
if (process.env.QUANUX_figma) {
    process.env.FIGMA_API_KEY = process.env.QUANUX_figma;
}

if (!process.env.FIGMA_API_KEY) {
    console.warn("Warning: FIGMA_API_KEY is not set.");
    console.warn("Either FIGMA_API_KEY or FIGMA_OAUTH_TOKEN is required.");
}

// 3. Spawn
const child = spawn(binPath, process.argv.slice(2), {
    stdio: 'inherit',
    env: process.env // Pass through all env vars including injected ones
});

child.on('error', (err) => {
    console.error("Failed to start Figma MCP Server:", err);
    process.exit(1);
});

child.on('exit', (code) => {
    process.exit(code ?? 0);
});
