MCP Server - shadcn/ui

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

# MCP Server

Copy Page

[Previous](/docs/directory) [Next](/docs/new)

Use the shadcn MCP server to browse, search, and install components from registries.

The shadcn MCP Server allows AI assistants to interact with items from registries. You can browse available components, search for specific ones, and install them directly into your project using natural language.

For example, you can ask an AI assistant to "Build a landing page using components from the acme registry" or "Find me a login form from the shadcn registry".

Registries are configured in your project's `components.json` file.

components.json

```
Copy{
  "registries": {
    "@acme": "https://acme.com/r/{name}.json"
  }
}
```

* * *

## Quick Start

Select your MCP client and follow the instructions to configure the shadcn MCP server. If you'd like to do it manually, see the [Configuration](#configuration) section.

Claude CodeCursorVS CodeCodexOpenCode

**Run the following command** in your project:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest mcp init --client claude

Copy
```

**Restart Claude Code** and try the following prompts:

- Show me all available components in the shadcn registry
- Add the button, dialog and card components to my project
- Create a contact form using components from the shadcn registry

**Note:** You can use `/mcp` command in Claude Code to debug the MCP server.

* * *

## What is MCP?

[Model Context Protocol (MCP)](https://modelcontextprotocol.io) is an open protocol that enables AI assistants to securely connect to external data sources and tools. With the shadcn MCP server, your AI assistant gains direct access to:

- **Browse Components** \- List all available components, blocks, and templates from any configured registry
- **Search Across Registries** \- Find specific components by name or functionality across multiple sources
- **Install with Natural Language** \- Add components using simple conversational prompts like "add a login form"
- **Support for Multiple Registries** \- Access public registries, private company libraries, and third-party sources

* * *

## How It Works

The MCP server acts as a bridge between your AI assistant, component registries and the shadcn CLI.

1. **Registry Connection** \- MCP connects to configured registries (shadcn/ui, private registries, third-party sources)
2. **Natural Language** \- You describe what you need in plain English
3. **AI Processing** \- The assistant translates your request into registry commands
4. **Component Delivery** \- Resources are fetched and installed in your project

* * *

## Supported Registries

The shadcn MCP server works out of the box with any shadcn-compatible registry.

- **shadcn/ui Registry** \- The default registry with all shadcn/ui components
- **Third-Party Registries** \- Any registry following the shadcn registry specification
- **Private Registries** \- Your company's internal component libraries
- **Namespaced Registries** \- Multiple registries configured with `@namespace` syntax

* * *

## Configuration

You can use any MCP client to interact with the shadcn MCP server. Here are the instructions for the most popular ones.

### Claude Code

To use the shadcn MCP server with Claude Code, add the following configuration to your project's `.mcp.json` file:

.mcp.json

```
Copy{
  "mcpServers": {
    "shadcn": {
      "command": "npx",
      "args": ["shadcn@latest", "mcp"]
    }
  }
}
```

After adding the configuration, restart Claude Code and run `/mcp` to see the shadcn MCP server in the list. If you see `Connected`, you're good to go.

See the [Claude Code MCP documentation](https://code.claude.com/docs/en/mcp) for more details.

### Cursor

To configure MCP in Cursor, add the shadcn server to your project's `.cursor/mcp.json` configuration file:

.cursor/mcp.json

```
Copy{
  "mcpServers": {
    "shadcn": {
      "command": "npx",
      "args": ["shadcn@latest", "mcp"]
    }
  }
}
```

After adding the configuration, enable the shadcn MCP server in Cursor Settings.

Once enabled, you should see a green dot next to the shadcn server in the MCP server list and a list of available tools.

See the [Cursor MCP documentation](https://docs.cursor.com/en/context/mcp#using-mcp-json) for more details.

### VS Code

To configure MCP in VS Code with GitHub Copilot, add the shadcn server to your project's `.vscode/mcp.json` configuration file:

.vscode/mcp.json

```
Copy{
  "servers": {
    "shadcn": {
      "command": "npx",
      "args": ["shadcn@latest", "mcp"]
    }
  }
}
```

After adding the configuration, open `.vscode/mcp.json` and click **Start** next to the shadcn server.

See the [VS Code MCP documentation](https://code.visualstudio.com/docs/copilot/chat/mcp-servers) for more details.

### Codex

**Note:** The `shadcn` CLI cannot automatically update `~/.codex/config.toml`.
You'll need to add the configuration manually.

To configure MCP in Codex, add the shadcn server to `~/.codex/config.toml`:

~/.codex/config.toml

```
Copy[mcp_servers.shadcn]
command = "npx"
args = ["shadcn@latest", "mcp"]
```

After adding the configuration, restart Codex to load the MCP server.

* * *

## Configuring Registries

The MCP server supports multiple registries through your project's `components.json` configuration. This allows you to access components from various sources including private registries and third-party providers.

Configure additional registries in your `components.json`:

components.json

```
Copy{
  "registries": {
    "@acme": "https://registry.acme.com/{name}.json",
    "@internal": {
      "url": "https://internal.company.com/{name}.json",
      "headers": {
        "Authorization": "Bearer ${REGISTRY_TOKEN}"
      }
    }
  }
}
```

**Note:** No configuration is needed to access the standard shadcn/ui
registry.

* * *

## Authentication

For private registries requiring authentication, set environment variables in your `.env.local`:

.env.local

```
CopyREGISTRY_TOKEN=your_token_here
API_KEY=your_api_key_here
```

For more details on registry authentication, see the [Authentication documentation](/docs/registry/authentication).

* * *

## Example Prompts

Once the MCP server is configured, you can use natural language to interact with registries. Try one of the following prompts:

### Browse & Search

- Show me all available components in the shadcn registry
- Find me a login form from the shadcn registry

### Install Items

- Add the button component to my project
- Create a login form using shadcn components
- Install the Cursor rules from the acme registry

### Work with Namespaces

- Show me components from acme registry
- Install @internal/auth-form
- Build me a landing page using hero, features and testimonials sections from the acme registry

* * *

## Troubleshooting

### MCP Not Responding

If the MCP server isn't responding to prompts:

1. **Check Configuration** \- Verify the MCP server is properly configured and enabled in your MCP client
2. **Restart MCP Client** \- Restart your MCP client after configuration changes
3. **Verify Installation** \- Ensure `shadcn` is installed in your project
4. **Check Network** \- Confirm you can access the configured registries

### Registry Access Issues

If components aren't loading from registries:

1. **Check components.json** \- Verify registry URLs are correct
2. **Test Authentication** \- Ensure environment variables are set for private registries
3. **Verify Registry** \- Confirm the registry is online and accessible
4. **Check Namespace** \- Ensure namespace syntax is correct ( `@namespace/component`)

### Installation Failures

If components fail to install:

1. **Check Project Setup** \- Ensure you have a valid `components.json` file
2. **Verify Paths** \- Confirm the target directories exist
3. **Check Permissions** \- Ensure write permissions for component directories
4. **Review Dependencies** \- Check that required dependencies are installed

### No Tools or Prompts

If you see the `No tools or prompts` message, try the following:

1. **Clear the npx cache** \- Run `npx clear-npx-cache`
2. **Re-enable the MCP server** \- Try to re-enable the MCP server in your MCP client
3. **Check Logs** \- In Cursor, you can see the logs under View -> Output and select `MCP: project-*` in the dropdown.

* * *

## Learn More

- [Registry Documentation](/docs/registry) \- Complete guide to shadcn registries
- [Namespaces](/docs/registry/namespace) \- Configure multiple registry sources
- [Authentication](/docs/registry/authentication) \- Secure your private registries
- [MCP Specification](https://modelcontextprotocol.io) \- Learn about Model Context Protocol

[Registry Directory](/docs/directory) [Your project is ready!](/docs/new)

On This Page

[Quick Start](#quick-start) [What is MCP?](#what-is-mcp) [How It Works](#how-it-works) [Supported Registries](#supported-registries) [Configuration](#configuration) [Claude Code](#claude-code) [Cursor](#cursor) [VS Code](#vs-code) [Codex](#codex) [Configuring Registries](#configuring-registries) [Authentication](#authentication) [Example Prompts](#example-prompts) [Browse & Search](#browse--search) [Install Items](#install-items) [Work with Namespaces](#work-with-namespaces) [Troubleshooting](#troubleshooting) [MCP Not Responding](#mcp-not-responding) [Registry Access Issues](#registry-access-issues) [Installation Failures](#installation-failures) [No Tools or Prompts](#no-tools-or-prompts) [Learn More](#learn-more)

Deploy your shadcn/ui app on Vercel

Trusted by OpenAI, Sonos, Adobe, and more.

Vercel provides tools and infrastructure to deploy apps and features at scale.

Deploy Now [Deploy to Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout)

Built by [shadcn](https://twitter.com/shadcn) at [Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout). The source code is available on [GitHub](https://github.com/shadcn-ui/ui).

Create Project