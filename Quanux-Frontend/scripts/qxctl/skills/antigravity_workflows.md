antigravity-workflows by sickn33/antigravity-awesome-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [sickn33](/sickn33)/ [antigravity-awesome-skills](/sickn33/antigravity-awesome-skills)/antigravity-workflows

# antigravity-workflows

`$ npx skills add https://github.com/sickn33/antigravity-awesome-skills --skill antigravity-workflows`

SKILL.md

# Antigravity Workflows

Use this skill to turn a complex objective into a guided sequence of skill invocations.

## When to Use This Skill

Use this skill when:

- The user wants to combine several skills without manually selecting each one.
- The goal is multi-phase (for example: plan, build, test, ship).
- The user asks for best-practice execution for common scenarios like:
  - Shipping a SaaS MVP
  - Running a web security audit
  - Building an AI agent system
  - Implementing browser automation and E2E QA

## Workflow Source of Truth

Read workflows in this order:

1. `docs/WORKFLOWS.md` for human-readable playbooks.
2. `data/workflows.json` for machine-readable workflow metadata.

## How to Run This Skill

1. Identify the user's concrete outcome.
2. Propose the 1-2 best matching workflows.
3. Ask the user to choose one.
4. Execute step-by-step:
   - Announce current step and expected artifact.
   - Invoke recommended skills for that step.
   - Verify completion criteria before moving to next step.
5. At the end, provide:
   - Completed artifacts
   - Validation evidence
   - Remaining risks and next actions

## Default Workflow Routing

- Product delivery request -> `ship-saas-mvp`
- Security review request -> `security-audit-web-app`
- Agent/LLM product request -> `build-ai-agent-system`
- E2E/browser testing request -> `qa-browser-automation`
- Domain-driven design request -> `design-ddd-core-domain`

## Copy-Paste Prompts

```text code-highlight
Use @antigravity-workflows to run the "Ship a SaaS MVP" workflow for my project idea.

```

```text code-highlight
Use @antigravity-workflows and execute a full "Security Audit for a Web App" workflow.

```

```text code-highlight
Use @antigravity-workflows to guide me through "Build an AI Agent System" with checkpoints.

```

```text code-highlight
Use @antigravity-workflows to execute the "QA and Browser Automation" workflow and stabilize flaky tests.

```

```text code-highlight
Use @antigravity-workflows to execute the "Design a DDD Core Domain" workflow for my new service.

```

## Limitations

- This skill orchestrates; it does not replace specialized skills.
- It depends on the local availability of referenced skills.
- It does not guarantee success without environment access, credentials, or required infrastructure.
- For stack-specific browser automation in Go, `go-playwright` may require the corresponding skill to be present in your local skills repository.

## Related Skills

- `concise-planning`
- `brainstorming`
- `workflow-automation`
- `verification-before-completion`

Weekly Installs

302

Repository

[sickn33/antigra…e-skills](https://github.com/sickn33/antigravity-awesome-skills "sickn33/antigravity-awesome-skills")

GitHub Stars

28.9K

First Seen

Feb 10, 2026

Security Audits

[Gen Agent Trust HubPass](/sickn33/antigravity-awesome-skills/antigravity-workflows/security/agent-trust-hub) [SocketPass](/sickn33/antigravity-awesome-skills/antigravity-workflows/security/socket) [SnykPass](/sickn33/antigravity-awesome-skills/antigravity-workflows/security/snyk)

Installed on

codex296

opencode296

gemini-cli295

github-copilot295

kimi-cli294

amp293