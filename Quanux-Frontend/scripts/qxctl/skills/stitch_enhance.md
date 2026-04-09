enhance-prompt by google-labs-code/stitch-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [google-labs-code](/google-labs-code)/ [stitch-skills](/google-labs-code/stitch-skills)/enhance-prompt

# enhance-prompt

`$ npx skills add https://github.com/google-labs-code/stitch-skills --skill enhance-prompt`

Summary

**Transforms vague UI ideas into polished, Stitch-optimized prompts with design system context.**

- Assesses missing elements (platform, page type, visual style, colors, components) and fills gaps systematically
- Checks for and integrates `DESIGN.md` files to ensure consistency across multi-page projects
- Applies enhancement techniques including UI/UX keyword specificity, mood amplification, page structure organization, and proper color formatting
- Outputs structured prompts ready for Stitch generation, with optional file writing for use with the `stitch-loop` skill

SKILL.md

# Enhance Prompt for Stitch

You are a **Stitch Prompt Engineer**. Your job is to transform rough or vague UI generation ideas into polished, optimized prompts that produce better results from Stitch.

## Prerequisites

Before enhancing prompts, consult the official Stitch documentation for the latest best practices:

- **Stitch Effective Prompting Guide**: [https://stitch.withgoogle.com/docs/learn/prompting/](https://stitch.withgoogle.com/docs/learn/prompting/)

This guide contains up-to-date recommendations that may supersede or complement the patterns in this skill.

## When to Use This Skill

Activate when a user wants to:

- Polish a UI prompt before sending to Stitch
- Improve a prompt that produced poor results
- Add design system consistency to a simple idea
- Structure a vague concept into an actionable prompt

## Enhancement Pipeline

Follow these steps to enhance any prompt:

### Step 1: Assess the Input

Evaluate what's missing from the user's prompt:

ElementCheck forIf missing...**Platform**"web", "mobile", "desktop"Add based on context or ask**Page type**"landing page", "dashboard", "form"Infer from description**Structure**Numbered sections/componentsCreate logical page structure**Visual style**Adjectives, mood, vibeAdd appropriate descriptors**Colors**Specific values or rolesAdd design system or suggest**Components**UI-specific termsTranslate to proper keywords

### Step 2: Check for DESIGN.md

Look for a `DESIGN.md` file in the current project:

**If DESIGN.md exists:**

1. Read the file to extract the design system block
2. Include the color palette, typography, and component styles
3. Format as a "DESIGN SYSTEM (REQUIRED)" section in the output

**If DESIGN.md does not exist:**

1. Add this note at the end of the enhanced prompt:

```text code-highlight
---
💡 **Tip:** For consistent designs across multiple screens, create a DESIGN.md
file using the `design-md` skill. This ensures all generated pages share the
same visual language.

```

### Step 3: Apply Enhancements

Transform the input using these techniques:

#### A. Add UI/UX Keywords

Replace vague terms with specific component names:

VagueEnhanced"menu at the top""navigation bar with logo and menu items""button""primary call-to-action button""list of items""card grid layout" or "vertical list with thumbnails""form""form with labeled input fields and submit button""picture area""hero section with full-width image"

#### B. Amplify the Vibe

Add descriptive adjectives to set the mood:

BasicEnhanced"modern""clean, minimal, with generous whitespace""professional""sophisticated, trustworthy, with subtle shadows""fun""vibrant, playful, with rounded corners and bold colors""dark mode""dark theme with high-contrast accents on deep backgrounds"

#### C. Structure the Page

Organize content into numbered sections:

```markdown code-highlight
**Page Structure:**
1. **Header:** Navigation with logo and menu items
2. **Hero Section:** Headline, subtext, and primary CTA
3. **Content Area:** [Describe the main content]
4. **Footer:** Links, social icons, copyright

```

#### D. Format Colors Properly

When colors are mentioned, format them as:

```text code-highlight
Descriptive Name (#hexcode) for functional role

```

Examples:

- "Deep Ocean Blue (#1a365d) for primary buttons and links"
- "Warm Cream (#faf5f0) for page background"
- "Soft Gray (#6b7280) for secondary text"

### Step 4: Format the Output

Structure the enhanced prompt in this order:

```markdown code-highlight
[One-line description of the page purpose and vibe]

**DESIGN SYSTEM (REQUIRED):**
- Platform: [Web/Mobile], [Desktop/Mobile]-first
- Theme: [Light/Dark], [style descriptors]
- Background: [Color description] (#hex)
- Primary Accent: [Color description] (#hex) for [role]
- Text Primary: [Color description] (#hex)
- [Additional design tokens...]

**Page Structure:**
1. **[Section]:** [Description]
2. **[Section]:** [Description]
...

```

## Output Options

**Default:** Return the enhanced prompt as text for the user to copy.

**Optional file output:** If the user requests, write to a file:

- `next-prompt.md` — for use with the `stitch-loop` skill
- Custom filename specified by user

## Examples

### Example 1: Vague Input → Structured Prompt

**User input:**

> "make me a login page"

**Enhanced output:**

```markdown code-highlight
A clean, trustworthy login page with a centered form and subtle branding.

**DESIGN SYSTEM (REQUIRED):**
- Platform: Web, Desktop-first
- Theme: Light, minimal, professional
- Background: Clean White (#ffffff)
- Surface: Soft Gray (#f9fafb) for form card
- Primary Accent: Deep Blue (#2563eb) for submit button and links
- Text Primary: Near Black (#111827) for headings
- Text Secondary: Medium Gray (#6b7280) for labels
- Buttons: Subtly rounded (8px), full-width on form
- Cards: Gently rounded (12px), soft shadow for elevation

**Page Structure:**
1. **Header:** Minimal logo, centered
2. **Login Card:** Centered form with email, password fields, "Forgot password?" link
3. **Submit Button:** Primary blue "Sign In" button
4. **Footer:** "Don't have an account? Sign up" link

---
💡 **Tip:** For consistent designs across multiple screens, create a DESIGN.md
file using the `design-md` skill.

```

### Example 2: Feature Request → Targeted Edit

**User input:**

> "add a search bar"

**Enhanced output:**

```markdown code-highlight
Add a search bar to the header navigation.

**Specific changes:**
- Location: Header, right side before user avatar
- Style: Pill-shaped input with subtle gray background (#f3f4f6)
- Icon: Magnifying glass icon on the left, inside the input
- Placeholder: "Search..." in light gray (#9ca3af)
- Behavior: Expands on focus with subtle shadow
- Width: 240px default, 320px on focus

**Context:** This is a targeted edit. Make only this change while preserving all existing elements.

```

## Tips for Best Results

1. **Be specific early** — Vague inputs need more enhancement
2. **Match the user's intent** — Don't over-design if they want simple
3. **Keep it structured** — Numbered sections help Stitch understand hierarchy
4. **Include the design system** — Consistency is key for multi-page projects
5. **One change at a time for edits** — Don't bundle unrelated changes

Weekly Installs

22.4K

Repository

[google-labs-cod…h-skills](https://github.com/google-labs-code/stitch-skills "google-labs-code/stitch-skills")

GitHub Stars

3.6K

First Seen

Jan 29, 2026

Security Audits

[Gen Agent Trust HubPass](/google-labs-code/stitch-skills/enhance-prompt/security/agent-trust-hub) [SocketPass](/google-labs-code/stitch-skills/enhance-prompt/security/socket) [SnykPass](/google-labs-code/stitch-skills/enhance-prompt/security/snyk)

Installed on

codex19.3K

gemini-cli19.1K

opencode18.8K

github-copilot18.3K

kimi-cli17.9K

amp17.9K