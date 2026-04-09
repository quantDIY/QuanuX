web-design-guidelines by vercel-labs/agent-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [vercel-labs](/vercel-labs)/ [agent-skills](/vercel-labs/agent-skills)/web-design-guidelines

# web-design-guidelines

`$ npx skills add https://github.com/vercel-labs/agent-skills --skill web-design-guidelines`

Summary

**Audit UI code against Vercel's Web Interface Guidelines for design and accessibility compliance.**

- Fetches the latest guidelines from a remote source before each review, ensuring rules stay current
- Accepts file paths or patterns as arguments; prompts for files if none provided
- Outputs findings in a terse `file:line` format for quick scanning and remediation
- Covers design, accessibility, and UX best practices as defined in the guidelines repository

SKILL.md

# Web Interface Guidelines

Review files for compliance with Web Interface Guidelines.

## How It Works

1. Fetch the latest guidelines from the source URL below
2. Read the specified files (or prompt user for files/pattern)
3. Check against all rules in the fetched guidelines
4. Output findings in the terse `file:line` format

## Guidelines Source

Fetch fresh guidelines before each review:

```text code-highlight
https://raw.githubusercontent.com/vercel-labs/web-interface-guidelines/main/command.md

```

Use WebFetch to retrieve the latest rules. The fetched content contains all the rules and output format instructions.

## Usage

When a user provides a file or pattern argument:

1. Fetch guidelines from the source URL above
2. Read the specified files
3. Apply all rules from the fetched guidelines
4. Output findings using the format specified in the guidelines

If no files specified, ask the user which files to review.

Weekly Installs

213.8K

Repository

[vercel-labs/agent-skills](https://github.com/vercel-labs/agent-skills "vercel-labs/agent-skills")

GitHub Stars

24.1K

First Seen

Jan 19, 2026

Security Audits

[Gen Agent Trust HubPass](/vercel-labs/agent-skills/web-design-guidelines/security/agent-trust-hub) [SocketPass](/vercel-labs/agent-skills/web-design-guidelines/security/socket) [SnykWarn](/vercel-labs/agent-skills/web-design-guidelines/security/snyk)

Installed on

opencode158.9K

codex155.2K

gemini-cli155.2K

github-copilot146.3K

cursor142.1K

amp127.2K