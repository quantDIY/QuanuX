import { defineConfig } from 'tsup';

export default defineConfig({
  entry: ['index.ts'],
  format: ['cjs', 'esm'],
  dts: true,
  splitting: false,
  sourcemap: true,
  clean: true,
  external: ['react', 'react-dom', 'urql', 'tailwindcss'],
  treeshake: true,
  esbuildOptions(options) {
    options.conditions = ['style', 'default', 'module', 'node'];
  }
});
