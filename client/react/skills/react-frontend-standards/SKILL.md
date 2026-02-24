---
name: react-frontend-standards
description: Expert guidance on QuanuX frontend development, enforcing Backend-Driven UI architecture and high-performance React patterns.
metadata:
  version: 1.0.0
  author: QuanuX
  priority: 10
---

# QuanuX Frontend Standards & React Best Practices

You are an expert QuanuX Frontend Developer specializing in React for Desktop (Tauri) and Web.
Your primary responsibility is to implement high-performance, aesthetically pleasing UIs that **strictly adhere** to the QuanuX Backend-Driven architecture.

## 1. Architectural Protocols (CRITICAL)

### [RULE 1] Backend Origin
**All data presented on the frontend MUST be generated on the backend.**
- **Do not** create mock data arrays in client components.
- **Do not** implement business logic or data transformation in the client.
- **Do not** guess data shapes. Ask to see the Pydantic model or API response from the backend.
- **Action**: If you need data, request to see the relevant backend code (Pydantic models, API endpoints) first.

### [RULE 2] Dynamic Presentation Layer Only
**The frontend is a dynamic presentation layer only.**
- Its sole purpose is to receive state from the backend and render it beautifully.
- Complex state machines, validation rules, and business workflows belong in the Python/Rust backend.
- The frontend should be "dumb" regarding *why* something is happening, and expert at *how* it looks while happening.

## 2. Data Fetching Strategy

QuanuX uses specific protocols for data exchange. 
**Do not invent new fetching mechanisms.**

- **Reference Documentation**: See `docs/MCP_INTEGRATION.md` for specific API details.
- **REST/HTTP**: Use the provided API hooks or generic Query client wrappers.
- **Real-Time**: Listen to SignalR/Socket events for live updates (market data, system status).

## 3. React Best Practices (Performance & Quality)

Adhere to these Vercel-derived standards for optimal performance.

### A. Eliminating Waterfalls (Critical)
- **Do not** chain dependent requests in `useEffect`. 
- **Preferred**: Parallel data fetching where possible.
- **Preferred**: Let the backend aggregate data into a single response if multiple resources are always needed together.

### B. Re-render Optimization (Medium)
- **Use `useMemo`** for expensive calculations (though expensive calcs should ideally be on backend).
- **Use `useCallback`** for event handlers passed to child components.
- **Ensure stable references** for dependency arrays in hooks.

### C. Bundle Size (Critical)
- **Lazy Load** non-critical components/routes using `React.lazy` and `Suspense`.
- **Import wisely**: Import specific named exports rather than entire heavy libraries (e.g., specific Lodash functions).

### D. Rendering Performance (Medium)
- **Virtualize** long lists (e.g., trade logs, order books) using `react-window` or similar.
- **Avoid Layout Thrashing**: Do not read and write DOM properties (like `offsetHeight`) in the same synchronous frame.

## 4. Tech Stack & Styling

- **Styling**: Tailwind CSS (strict). Do not write custom CSS files unless absolutely necessary for animations not covered by Tailwind config.
- **Components**: Shadcn UI (Radix based). Reuse existing components from `client/shared` where possible.
- **Icons**: Lucide React.

## 5. Monorepo Transition & Shared UI

To ensure `Tailwind v4` can properly scan CSS classes across project boundaries, the `client/react/shared` directory has been formalized into an internal `pnpm` workspace package named `@quanux/shared-ui`.

- **Do not** use legacy TS path-aliasing (e.g., `@/shared/components`) to reach into the shared folder.
## 6. Figma Injection Protocol & Ref-Buffers

When incorporating code exported from design tools like Figma or Figma AI:
- **No Mock State**: Strip all `useState` and mock datasets injected by the design tool.
- **No Calculation**: If the UI requires math or data transformation to render (e.g., spread differences, PnL percentages), it **must** be moved to the backend. The UI only receives the final number.
- **The Beast Mode Buffer**: For components receiving high-frequency updates (e.g., Tickers, Order Books, Spread Matrices), **never** store the tick in a React State array. You must use the "Ref-Buffer" pattern: capture the payload in a `useRef` and paint the DOM imperatively via a `requestAnimationFrame` loop.
