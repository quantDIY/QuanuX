vercel-react-best-practices by vercel-labs/agent-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [vercel-labs](/vercel-labs)/ [agent-skills](/vercel-labs/agent-skills)/vercel-react-best-practices

# vercel-react-best-practices

`$ npx skills add https://github.com/vercel-labs/agent-skills --skill vercel-react-best-practices`

Summary

**React and Next.js performance optimization guide with 64 prioritized rules across 8 categories.**

- Organized by impact level, from critical waterfalls and bundle optimization down to advanced patterns, each rule includes incorrect/correct code examples and explanations
- Covers eight domains: async patterns, bundle size, server-side caching, client-side data fetching, re-render optimization, rendering performance, JavaScript efficiency, and advanced patterns
- Designed for use during component writing, code review, refactoring, and performance audits on React and Next.js applications
- Each rule has a prefix code (e.g., `async-parallel`, `bundle-barrel-imports`) for easy reference in automated tooling and documentation

SKILL.md

# Vercel React Best Practices

Comprehensive performance optimization guide for React and Next.js applications, maintained by Vercel. Contains 67 rules across 8 categories, prioritized by impact to guide automated refactoring and code generation.

## When to Apply

Reference these guidelines when:

- Writing new React components or Next.js pages
- Implementing data fetching (client or server-side)
- Reviewing code for performance issues
- Refactoring existing React/Next.js code
- Optimizing bundle size or load times

## Rule Categories by Priority

PriorityCategoryImpactPrefix1Eliminating WaterfallsCRITICAL`async-`2Bundle Size OptimizationCRITICAL`bundle-`3Server-Side PerformanceHIGH`server-`4Client-Side Data FetchingMEDIUM-HIGH`client-`5Re-render OptimizationMEDIUM`rerender-`6Rendering PerformanceMEDIUM`rendering-`7JavaScript PerformanceLOW-MEDIUM`js-`8Advanced PatternsLOW`advanced-`

## Quick Reference

### 1\. Eliminating Waterfalls (CRITICAL)

- `async-cheap-condition-before-await` \- Check cheap sync conditions before awaiting flags or remote values
- `async-defer-await` \- Move await into branches where actually used
- `async-parallel` \- Use Promise.all() for independent operations
- `async-dependencies` \- Use better-all for partial dependencies
- `async-api-routes` \- Start promises early, await late in API routes
- `async-suspense-boundaries` \- Use Suspense to stream content

### 2\. Bundle Size Optimization (CRITICAL)

- `bundle-barrel-imports` \- Import directly, avoid barrel files
- `bundle-dynamic-imports` \- Use next/dynamic for heavy components
- `bundle-defer-third-party` \- Load analytics/logging after hydration
- `bundle-conditional` \- Load modules only when feature is activated
- `bundle-preload` \- Preload on hover/focus for perceived speed

### 3\. Server-Side Performance (HIGH)

- `server-auth-actions` \- Authenticate server actions like API routes
- `server-cache-react` \- Use React.cache() for per-request deduplication
- `server-cache-lru` \- Use LRU cache for cross-request caching
- `server-dedup-props` \- Avoid duplicate serialization in RSC props
- `server-hoist-static-io` \- Hoist static I/O (fonts, logos) to module level
- `server-serialization` \- Minimize data passed to client components
- `server-parallel-fetching` \- Restructure components to parallelize fetches
- `server-parallel-nested-fetching` \- Chain nested fetches per item in Promise.all
- `server-after-nonblocking` \- Use after() for non-blocking operations

### 4\. Client-Side Data Fetching (MEDIUM-HIGH)

- `client-swr-dedup` \- Use SWR for automatic request deduplication
- `client-event-listeners` \- Deduplicate global event listeners
- `client-passive-event-listeners` \- Use passive listeners for scroll
- `client-localstorage-schema` \- Version and minimize localStorage data

### 5\. Re-render Optimization (MEDIUM)

- `rerender-defer-reads` \- Don't subscribe to state only used in callbacks
- `rerender-memo` \- Extract expensive work into memoized components
- `rerender-memo-with-default-value` \- Hoist default non-primitive props
- `rerender-dependencies` \- Use primitive dependencies in effects
- `rerender-derived-state` \- Subscribe to derived booleans, not raw values
- `rerender-derived-state-no-effect` \- Derive state during render, not effects
- `rerender-functional-setstate` \- Use functional setState for stable callbacks
- `rerender-lazy-state-init` \- Pass function to useState for expensive values
- `rerender-simple-expression-in-memo` \- Avoid memo for simple primitives
- `rerender-split-combined-hooks` \- Split hooks with independent dependencies
- `rerender-move-effect-to-event` \- Put interaction logic in event handlers
- `rerender-transitions` \- Use startTransition for non-urgent updates
- `rerender-use-deferred-value` \- Defer expensive renders to keep input responsive
- `rerender-use-ref-transient-values` \- Use refs for transient frequent values
- `rerender-no-inline-components` \- Don't define components inside components

### 6\. Rendering Performance (MEDIUM)

- `rendering-animate-svg-wrapper` \- Animate div wrapper, not SVG element
- `rendering-content-visibility` \- Use content-visibility for long lists
- `rendering-hoist-jsx` \- Extract static JSX outside components
- `rendering-svg-precision` \- Reduce SVG coordinate precision
- `rendering-hydration-no-flicker` \- Use inline script for client-only data
- `rendering-hydration-suppress-warning` \- Suppress expected mismatches
- `rendering-activity` \- Use Activity component for show/hide
- `rendering-conditional-render` \- Use ternary, not && for conditionals
- `rendering-usetransition-loading` \- Prefer useTransition for loading state
- `rendering-resource-hints` \- Use React DOM resource hints for preloading
- `rendering-script-defer-async` \- Use defer or async on script tags

### 7\. JavaScript Performance (LOW-MEDIUM)

- `js-batch-dom-css` \- Group CSS changes via classes or cssText
- `js-index-maps` \- Build Map for repeated lookups
- `js-cache-property-access` \- Cache object properties in loops
- `js-cache-function-results` \- Cache function results in module-level Map
- `js-cache-storage` \- Cache localStorage/sessionStorage reads
- `js-combine-iterations` \- Combine multiple filter/map into one loop
- `js-length-check-first` \- Check array length before expensive comparison
- `js-early-exit` \- Return early from functions
- `js-hoist-regexp` \- Hoist RegExp creation outside loops
- `js-min-max-loop` \- Use loop for min/max instead of sort
- `js-set-map-lookups` \- Use Set/Map for O(1) lookups
- `js-tosorted-immutable` \- Use toSorted() for immutability
- `js-flatmap-filter` \- Use flatMap to map and filter in one pass
- `js-request-idle-callback` \- Defer non-critical work to browser idle time

### 8\. Advanced Patterns (LOW)

- `advanced-event-handler-refs` \- Store event handlers in refs
- `advanced-init-once` \- Initialize app once per app load
- `advanced-use-latest` \- useLatest for stable callback refs

## How to Use

Read individual rule files for detailed explanations and code examples:

```text code-highlight
rules/async-parallel.md
rules/bundle-barrel-imports.md

```

Each rule file contains:

- Brief explanation of why it matters
- Incorrect code example with explanation
- Correct code example with explanation
- Additional context and references

## Full Compiled Document

For the complete guide with all rules expanded: `AGENTS.md`

Weekly Installs

265.0K

Repository

[vercel-labs/agent-skills](https://github.com/vercel-labs/agent-skills "vercel-labs/agent-skills")

GitHub Stars

24.1K

First Seen

Jan 19, 2026

Security Audits

[Gen Agent Trust HubPass](/vercel-labs/agent-skills/vercel-react-best-practices/security/agent-trust-hub) [SocketPass](/vercel-labs/agent-skills/vercel-react-best-practices/security/socket) [SnykPass](/vercel-labs/agent-skills/vercel-react-best-practices/security/snyk)

Installed on

opencode195.5K

gemini-cli192.6K

codex192.4K

github-copilot182.1K

cursor177.0K

amp158.0K