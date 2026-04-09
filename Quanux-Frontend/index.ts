/**
 * QuanuX-Frontend Canonical Export
 * Provides a root-level aggregation of all strictly governed QuanuX packages.
 * This allows the entire monorepo to be installed and consumed as a unified package.
 */

// Re-export architectural packages
export * as UI from "./packages/ui/src";
export * as Network from "./packages/network/src";
