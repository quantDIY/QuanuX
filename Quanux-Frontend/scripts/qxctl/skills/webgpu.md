webgpu by cazala/webgpu-skill

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [cazala](/cazala)/ [webgpu-skill](/cazala/webgpu-skill)/webgpu

# webgpu

`$ npx skills add https://github.com/cazala/webgpu-skill --skill webgpu`

Summary

**Design, implement, and debug WebGPU applications, GPU compute pipelines, and WGSL shaders.**

- Covers WebGPU initialization, device setup, compute and render pipelines, shader authoring, and GPU/CPU synchronization strategies
- Includes architecture patterns for modular passes, phase-based simulation, spatial indexing, and capability fallback handling
- Provides guidance on performance optimization, safe readback strategies, and debugging practices for rendering, compute, ML workloads, and particle simulations
- Delivers minimal working code with buffer layouts, pass graphs, alignment notes, and optional reduced modes for unsupported platforms

SKILL.md

# WebGPU Skill

Use this skill to design, implement, and debug WebGPU applications and GPU compute pipelines. Keep it framework-agnostic and focus on reusable WebGPU/WGSL patterns.

## What this skill covers

- Cover WebGPU initialization, device setup, and surface configuration.
- Cover compute pipelines, workgroup sizing, and storage buffer layout.
- Cover render pipelines, render passes, and post-processing patterns.
- Cover GPU/CPU synchronization and safe readback strategies.
- Cover performance and debugging practices.
- Cover architecture patterns: modular passes, phase-based simulation, and capability handling.
- Cover use cases: rendering, compute, ML training/inference, grid simulations, and systems modeling.

## Core principles

- Choose a **capability strategy**: fallback runtime, reduced mode, or fail fast.
- Avoid full GPU readbacks in hot paths; use **localized queries** or small readback buffers.
- Structure simulation with **phases** (state, apply, integrate, constrain, correct) to keep WGSL cohesive.
- Use **spatial grids** or other spatial indexing for neighbor queries and high particle counts.
- Build **modular passes** so render and compute stages stay composable and testable.

## Workflow

When asked to build a WebGPU feature:

1. Confirm the target platform and WebGPU support expectations.
2. Propose a resource layout (buffers, textures, bind groups) with a simple data model.
3. Sketch the pipeline graph (compute vs render passes) and dependencies.
4. Provide minimal working code and scale up with performance constraints.
5. Choose a capability strategy when WebGPU is unavailable.

## Deliverable checklist

- Provide clean WebGPU init and error handling.
- Include a buffer layout with alignment notes (16-byte struct alignment for WGSL).
- Include a pass graph with clear read/write ownership (ping-pong textures if needed).
- Call out readback and when it is safe.
- Provide an optional fallback or reduced mode for critical functionality.

## References and assets

- Use [REFERENCE.md](https://github.com/cazala/webgpu-skill/blob/HEAD/REFERENCE.md) for a compact WebGPU cheat sheet.
- Use [references/](https://github.com/cazala/webgpu-skill/blob/HEAD/references/) for deeper patterns and concepts.
- Use [examples/](https://github.com/cazala/webgpu-skill/blob/HEAD/examples/) for runnable snippets.
- Use [templates/](https://github.com/cazala/webgpu-skill/blob/HEAD/templates/) for project scaffolds or starter code.

## Quick reference

See [REFERENCE.md](https://github.com/cazala/webgpu-skill/blob/HEAD/REFERENCE.md) for a compact WebGPU cheat sheet and [references/](https://github.com/cazala/webgpu-skill/blob/HEAD/references/) for deeper patterns, including [references/use-cases.md](https://github.com/cazala/webgpu-skill/blob/HEAD/references/use-cases.md) and [references/simulation-patterns.md](https://github.com/cazala/webgpu-skill/blob/HEAD/references/simulation-patterns.md).

Weekly Installs

443

Repository

[cazala/webgpu-skill](https://github.com/cazala/webgpu-skill "cazala/webgpu-skill")

GitHub Stars

12

First Seen

Jan 24, 2026

Security Audits

[Gen Agent Trust HubWarn](/cazala/webgpu-skill/webgpu/security/agent-trust-hub) [SocketPass](/cazala/webgpu-skill/webgpu/security/socket) [SnykWarn](/cazala/webgpu-skill/webgpu/security/snyk)

Installed on

opencode417

codex416

gemini-cli413

github-copilot399

claude-code397

cursor393