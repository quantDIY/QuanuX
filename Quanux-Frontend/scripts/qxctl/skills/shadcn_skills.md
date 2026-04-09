Skills - shadcn/ui

Toggle Menu
Menu [shadcn/ui](/) [Docs](/docs/installation) [Components](/docs/components) [Blocks](/blocks) [Charts](/charts/area) [Directory](/docs/directory) [Create](/create)

Search documentation...Search...

[111k](https://github.com/shadcn-ui/ui)

Toggle layout

Toggle theme

[New](/create)

Sections

- [Introduction](/docs)
- [Components](/docs/components)
- [Installation](/docs/installation)
- [Theming](/docs/theming)
- [CLI](/docs/cli)
- [RTL](/docs/rtl)
- [Skills](/docs/skills)
- [MCP Server](/docs/mcp)
- [Registry](/docs/registry)
- [Forms](/docs/forms)
- [Changelog](/docs/changelog)

Components

- [Accordion](/docs/components/radix/accordion)
- [Alert](/docs/components/radix/alert)
- [Alert Dialog](/docs/components/radix/alert-dialog)
- [Aspect Ratio](/docs/components/radix/aspect-ratio)
- [Avatar](/docs/components/radix/avatar)
- [Badge](/docs/components/radix/badge)
- [Breadcrumb](/docs/components/radix/breadcrumb)
- [Button](/docs/components/radix/button)
- [Button Group](/docs/components/radix/button-group)
- [Calendar](/docs/components/radix/calendar)
- [Card](/docs/components/radix/card)
- [Carousel](/docs/components/radix/carousel)
- [Chart](/docs/components/radix/chart)
- [Checkbox](/docs/components/radix/checkbox)
- [Collapsible](/docs/components/radix/collapsible)
- [Combobox](/docs/components/radix/combobox)
- [Command](/docs/components/radix/command)
- [Context Menu](/docs/components/radix/context-menu)
- [Data Table](/docs/components/radix/data-table)
- [Date Picker](/docs/components/radix/date-picker)
- [Dialog](/docs/components/radix/dialog)
- [Direction](/docs/components/radix/direction)
- [Drawer](/docs/components/radix/drawer)
- [Dropdown Menu](/docs/components/radix/dropdown-menu)
- [Empty](/docs/components/radix/empty)
- [Field](/docs/components/radix/field)
- [Hover Card](/docs/components/radix/hover-card)
- [Input](/docs/components/radix/input)
- [Input Group](/docs/components/radix/input-group)
- [Input OTP](/docs/components/radix/input-otp)
- [Item](/docs/components/radix/item)
- [Kbd](/docs/components/radix/kbd)
- [Label](/docs/components/radix/label)
- [Menubar](/docs/components/radix/menubar)
- [Native Select](/docs/components/radix/native-select)
- [Navigation Menu](/docs/components/radix/navigation-menu)
- [Pagination](/docs/components/radix/pagination)
- [Popover](/docs/components/radix/popover)
- [Progress](/docs/components/radix/progress)
- [Radio Group](/docs/components/radix/radio-group)
- [Resizable](/docs/components/radix/resizable)
- [Scroll Area](/docs/components/radix/scroll-area)
- [Select](/docs/components/radix/select)
- [Separator](/docs/components/radix/separator)
- [Sheet](/docs/components/radix/sheet)
- [Sidebar](/docs/components/radix/sidebar)
- [Skeleton](/docs/components/radix/skeleton)
- [Slider](/docs/components/radix/slider)
- [Sonner](/docs/components/radix/sonner)
- [Spinner](/docs/components/radix/spinner)
- [Switch](/docs/components/radix/switch)
- [Table](/docs/components/radix/table)
- [Tabs](/docs/components/radix/tabs)
- [Textarea](/docs/components/radix/textarea)
- [Toast](/docs/components/radix/toast)
- [Toggle](/docs/components/radix/toggle)
- [Toggle Group](/docs/components/radix/toggle-group)
- [Tooltip](/docs/components/radix/tooltip)
- [Typography](/docs/components/radix/typography)

Get Started

- [Installation](/docs/installation)
- [components.json](/docs/components-json)
- [Theming](/docs/theming)
- [Dark Mode](/docs/dark-mode)
- [CLI](/docs/cli)
- [Monorepo](/docs/monorepo)
- [Skills](/docs/skills)
- [Open in v0](/docs/v0)
- [JavaScript](/docs/javascript)
- [Figma](/docs/figma)
- [llms.txt](/llms.txt)
- [Legacy Docs](/docs/legacy)

Forms

- [React Hook Form](/docs/forms/react-hook-form)
- [TanStack Form](/docs/forms/tanstack-form)

Registry

- [Introduction](/docs/registry)
- [Getting Started](/docs/registry/getting-started)
- [Namespaces](/docs/registry/namespace)
- [Authentication](/docs/registry/authentication)
- [Examples](/docs/registry/examples)
- [MCP Server](/docs/registry/mcp)
- [Add a Registry](/docs/registry/registry-index)
- [Open in v0](/docs/registry/open-in-v0)
- [registry.json](/docs/registry/registry-json)
- [registry-item.json](/docs/registry/registry-item-json)

# Skills

Copy Page

[Previous](/docs/monorepo) [Next](/docs/v0)

Give your AI assistant deep knowledge of shadcn/ui components, patterns, and best practices.

Skills give AI assistants like Claude Code project-aware context about shadcn/ui. When installed, your AI assistant knows how to find, install, compose, and customize components using the correct APIs and patterns for your project.

For example, you can ask your AI assistant to:

- _"Add a login form with email and password fields."_
- _"Create a settings page with a form for updating profile information."_
- _"Build a dashboard with a sidebar, stats cards, and a data table."_
- _"Switch to --preset \[CODE\]"_
- _"Can you add a hero from @tailark?"_

The skill reads your project's `components.json` and provides the assistant with your framework, aliases, installed components, icon library, and base library so it can generate correct code on the first try.

* * *

## Install

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx skills add shadcn/ui

Copy
```

This installs the shadcn skill into your project. Once installed, your AI assistant automatically loads it when working with shadcn/ui components.

Learn more about skills at [skills.sh](https://skills.sh).

* * *

## What's Included

The skill provides your AI assistant with the following knowledge:

### Project Context

On every interaction, the skill runs `shadcn info --json` to get your project's configuration: framework, Tailwind version, aliases, base library ( `radix` or `base`), icon library, installed components, and resolved file paths.

### CLI Commands

Full reference for all CLI commands: `init`, `add`, `search`, `view`, `docs`, `diff`, `info`, and `build`. Includes flags, dry-run mode, smart merge workflows, presets, and templates.

### Theming and Customization

How CSS variables, OKLCH colors, dark mode, custom colors, border radius, and component variants work. Includes guidance for both Tailwind v3 and v4.

### Registry Authoring

How to build and publish custom component registries: `registry.json` format, item types, file objects, dependencies, CSS variables, building, hosting, and user configuration.

### MCP Server

Setup and tools for the shadcn MCP server, which lets AI assistants search, browse, and install components from registries.

* * *

## How It Works

1. **Project detection** — The skill activates when it finds a `components.json` file in your project.
2. **Context injection** — It runs `shadcn info --json` to read your project configuration and injects the result into the assistant's context.
3. **Pattern enforcement** — The assistant follows shadcn/ui composition rules: using `FieldGroup` for forms, `ToggleGroup` for option sets, semantic colors, and correct base-specific APIs.
4. **Component discovery** — The assistant uses `shadcn docs`, `shadcn search`, or MCP tools to find components and their documentation before generating code.

## Learn More

- [CLI](/docs/cli) — Full CLI command reference
- [MCP Server](/docs/mcp) — Connect the MCP server for registry access
- [Theming](/docs/theming) — CSS variables and customization
- [Registry](/docs/registry) — Building and publishing custom registries
- [skills.sh](https://skills.sh) — Learn more about AI skills

[Monorepo](/docs/monorepo) [Open in v0](/docs/v0)

On This Page

[Install](#install) [What's Included](#whats-included) [Project Context](#project-context) [CLI Commands](#cli-commands) [Theming and Customization](#theming-and-customization) [Registry Authoring](#registry-authoring) [MCP Server](#mcp-server) [How It Works](#how-it-works) [Learn More](#learn-more)

Deploy your shadcn/ui app on Vercel

Trusted by OpenAI, Sonos, Adobe, and more.

Vercel provides tools and infrastructure to deploy apps and features at scale.

Deploy Now [Deploy to Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout)

Built by [shadcn](https://twitter.com/shadcn) at [Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout). The source code is available on [GitHub](https://github.com/shadcn-ui/ui).

Create Project