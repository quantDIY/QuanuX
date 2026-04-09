find-skills by vercel-labs/skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [vercel-labs](/vercel-labs)/ [skills](/vercel-labs/skills)/find-skills

# find-skills

`$ npx skills add https://github.com/vercel-labs/skills --skill find-skills`

Summary

**Discover and install specialized agent skills from the open ecosystem when users need extended capabilities.**

- Helps identify relevant skills by domain and task when users ask "how do I do X" or "find a skill for X"
- Integrates with the Skills CLI ( `npx skills find`, `npx skills add`) to search, verify, and install packages from the skills.sh directory
- Recommends skills based on install count, source reputation, and GitHub stars to ensure quality before suggesting installation
- Presents skill options with install commands and links to skills.sh for user review and one-click installation

SKILL.md

# Find Skills

This skill helps you discover and install skills from the open agent skills ecosystem.

## When to Use This Skill

Use this skill when the user:

- Asks "how do I do X" where X might be a common task with an existing skill
- Says "find a skill for X" or "is there a skill for X"
- Asks "can you do X" where X is a specialized capability
- Expresses interest in extending agent capabilities
- Wants to search for tools, templates, or workflows
- Mentions they wish they had help with a specific domain (design, testing, deployment, etc.)

## What is the Skills CLI?

The Skills CLI ( `npx skills`) is the package manager for the open agent skills ecosystem. Skills are modular packages that extend agent capabilities with specialized knowledge, workflows, and tools.

**Key commands:**

- `npx skills find [query]` \- Search for skills interactively or by keyword
- `npx skills add <package>` \- Install a skill from GitHub or other sources
- `npx skills check` \- Check for skill updates
- `npx skills update` \- Update all installed skills

**Browse skills at:** [https://skills.sh/](https://skills.sh/)

## How to Help Users Find Skills

### Step 1: Understand What They Need

When a user asks for help with something, identify:

1. The domain (e.g., React, testing, design, deployment)
2. The specific task (e.g., writing tests, creating animations, reviewing PRs)
3. Whether this is a common enough task that a skill likely exists

### Step 2: Check the Leaderboard First

Before running a CLI search, check the [skills.sh leaderboard](https://skills.sh/) to see if a well-known skill already exists for the domain. The leaderboard ranks skills by total installs, surfacing the most popular and battle-tested options.

For example, top skills for web development include:

- `vercel-labs/agent-skills` — React, Next.js, web design (100K+ installs each)
- `anthropics/skills` — Frontend design, document processing (100K+ installs)

### Step 3: Search for Skills

If the leaderboard doesn't cover the user's need, run the find command:

```bash code-highlight
npx skills find [query]

```

For example:

- User asks "how do I make my React app faster?" → `npx skills find react performance`
- User asks "can you help me with PR reviews?" → `npx skills find pr review`
- User asks "I need to create a changelog" → `npx skills find changelog`

### Step 4: Verify Quality Before Recommending

**Do not recommend a skill based solely on search results.** Always verify:

1. **Install count** — Prefer skills with 1K+ installs. Be cautious with anything under 100.
2. **Source reputation** — Official sources ( `vercel-labs`, `anthropics`, `microsoft`) are more trustworthy than unknown authors.
3. **GitHub stars** — Check the source repository. A skill from a repo with <100 stars should be treated with skepticism.

### Step 5: Present Options to the User

When you find relevant skills, present them to the user with:

1. The skill name and what it does
2. The install count and source
3. The install command they can run
4. A link to learn more at skills.sh

Example response:

```text code-highlight
I found a skill that might help! The "react-best-practices" skill provides
React and Next.js performance optimization guidelines from Vercel Engineering.
(185K installs)

To install it:
npx skills add vercel-labs/agent-skills@react-best-practices

Learn more: https://skills.sh/vercel-labs/agent-skills/react-best-practices

```

### Step 6: Offer to Install

If the user wants to proceed, you can install the skill for them:

```bash code-highlight
npx skills add <owner/repo@skill> -g -y

```

The `-g` flag installs globally (user-level) and `-y` skips confirmation prompts.

## Common Skill Categories

When searching, consider these common categories:

CategoryExample QueriesWeb Developmentreact, nextjs, typescript, css, tailwindTestingtesting, jest, playwright, e2eDevOpsdeploy, docker, kubernetes, ci-cdDocumentationdocs, readme, changelog, api-docsCode Qualityreview, lint, refactor, best-practicesDesignui, ux, design-system, accessibilityProductivityworkflow, automation, git

## Tips for Effective Searches

1. **Use specific keywords**: "react testing" is better than just "testing"
2. **Try alternative terms**: If "deploy" doesn't work, try "deployment" or "ci-cd"
3. **Check popular sources**: Many skills come from `vercel-labs/agent-skills` or `ComposioHQ/awesome-claude-skills`

## When No Skills Are Found

If no relevant skills exist:

1. Acknowledge that no existing skill was found
2. Offer to help with the task directly using your general capabilities
3. Suggest the user could create their own skill with `npx skills init`

Example:

```text code-highlight
I searched for skills related to "xyz" but didn't find any matches.
I can still help you with this task directly! Would you like me to proceed?

If this is something you do often, you could create your own skill:
npx skills init my-xyz-skill

```

Weekly Installs

794.9K

Repository

[vercel-labs/skills](https://github.com/vercel-labs/skills "vercel-labs/skills")

GitHub Stars

12.4K

First Seen

Jan 26, 2026

Security Audits

[Gen Agent Trust HubPass](/vercel-labs/skills/find-skills/security/agent-trust-hub) [SocketPass](/vercel-labs/skills/find-skills/security/socket) [SnykWarn](/vercel-labs/skills/find-skills/security/snyk)

Installed on

opencode740.9K

codex737.2K

gemini-cli720.2K

github-copilot717.1K

amp708.6K

kimi-cli691.9K