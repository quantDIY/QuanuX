shadcn - shadcn/ui

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

# shadcn

Copy Page

[Previous](/docs/rtl) [Next](/docs/monorepo)

Use the shadcn CLI to add components to your project.

## init

Use the `init` command to initialize configuration and dependencies for a new project.

The `init` command installs dependencies, adds the `cn` util and configures CSS variables for the project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest init

Copy
```

**Options**

```
CopyUsage: shadcn init [options] [components...]

initialize your project and install dependencies

Arguments:
  components              name, url or local path to component

Options:
  -t, --template <template>  the template to use. (next, vite, start, react-router, laravel, astro)
  -b, --base <base>          the component library to use. (radix, base)
  -p, --preset [name]        use a preset configuration. (name, URL, or preset code)
  -n, --name <name>          the name for the new project.
  -d, --defaults             use default configuration. (default: false)
  -y, --yes                  skip confirmation prompt. (default: true)
  -f, --force                force overwrite of existing configuration. (default: false)
  -c, --cwd <cwd>            the working directory. defaults to the current directory.
  -s, --silent               mute output. (default: false)
  --monorepo                 scaffold a monorepo project.
  --no-monorepo              skip the monorepo prompt.
  --reinstall                re-install existing UI components.
  --no-reinstall             do not re-install existing UI components.
  --rtl                      enable RTL support.
  --no-rtl                   disable RTL support.
  --css-variables            use css variables for theming. (default: true)
  --no-css-variables         do not use css variables for theming.
  -h, --help                 display help for command
```

The `create` command is an alias for `init`:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest create

Copy
```

* * *

## add

Use the `add` command to add components and dependencies to your project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add [component]

Copy
```

**Options**

```
CopyUsage: shadcn add [options] [components...]

add a component to your project

Arguments:
  components           name, url or local path to component

Options:
  -y, --yes            skip confirmation prompt. (default: false)
  -o, --overwrite      overwrite existing files. (default: false)
  -c, --cwd <cwd>      the working directory. defaults to the current directory.
  -a, --all            add all available components (default: false)
  -p, --path <path>    the path to add the component to.
  -s, --silent         mute output. (default: false)
  --dry-run            preview changes without writing files. (default: false)
  --diff [path]        show diff for a file.
  --view [path]        show file contents.
  -h, --help           display help for command
```

* * *

## view

Use the `view` command to view items from the registry before installing them.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest view [item]

Copy
```

You can view multiple items at once:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest view button card dialog

Copy
```

Or view items from namespaced registries:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest view @acme/auth @v0/dashboard

Copy
```

**Options**

```
CopyUsage: shadcn view [options] <items...>

view items from the registry

Arguments:
  items            the item names or URLs to view

Options:
  -c, --cwd <cwd>  the working directory. defaults to the current directory.
  -h, --help       display help for command
```

* * *

## search

Use the `search` command to search for items from registries.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest search [registry]

Copy
```

You can search with a query:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest search @shadcn -q "button"

Copy
```

Or search multiple registries at once:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest search @shadcn @v0 @acme

Copy
```

The `list` command is an alias for `search`:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest list @acme

Copy
```

**Options**

```
CopyUsage: shadcn search|list [options] <registries...>

search items from registries

Arguments:
  registries             the registry names or urls to search items from. Names
                         must be prefixed with @.

Options:
  -c, --cwd <cwd>        the working directory. defaults to the current directory.
  -q, --query <query>    query string
  -l, --limit <number>   maximum number of items to display per registry (default: "100")
  -o, --offset <number>  number of items to skip (default: "0")
  -h, --help             display help for command
```

* * *

## build

Use the `build` command to generate the registry JSON files.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest build

Copy
```

This command reads the `registry.json` file and generates the registry JSON files in the `public/r` directory.

**Options**

```
CopyUsage: shadcn build [options] [registry]

build components for a shadcn registry

Arguments:
  registry             path to registry.json file (default: "./registry.json")

Options:
  -o, --output <path>  destination directory for json files (default: "./public/r")
  -c, --cwd <cwd>      the working directory. defaults to the current directory.
  -h, --help           display help for command
```

To customize the output directory, use the `--output` option.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest build --output ./public/registry

Copy
```

* * *

## docs

Use the `docs` command to fetch documentation and API references for components.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest docs [component]

Copy
```

**Options**

```
CopyUsage: shadcn docs [options] [component]

fetch documentation and API references for components

Arguments:
  component          the component to get docs for

Options:
  -c, --cwd <cwd>    the working directory. defaults to the current directory.
  -b, --base <base>  the base to use either 'base' or 'radix'. defaults to project base.
  --json             output as JSON. (default: false)
  -h, --help         display help for command
```

* * *

## info

Use the `info` command to get information about your project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest info

Copy
```

**Options**

```
CopyUsage: shadcn info [options]

get information about your project

Options:
  -c, --cwd <cwd>  the working directory. defaults to the current directory.
  --json            output as JSON. (default: false)
  -h, --help        display help for command
```

* * *

## migrate

Use the `migrate` command to run migrations on your project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest migrate [migration]

Copy
```

**Available Migrations**

MigrationDescription`icons`Migrate your UI components to a different icon library.`radix`Migrate to radix-ui.`rtl`Migrate your components to support RTL (right-to-left).

**Options**

```
CopyUsage: shadcn migrate [options] [migration] [path]

run a migration.

Arguments:
  migration        the migration to run.
  path             optional path or glob pattern to migrate.

Options:
  -c, --cwd <cwd>  the working directory. defaults to the current directory.
  -l, --list       list all migrations. (default: false)
  -y, --yes        skip confirmation prompt. (default: false)
  -h, --help       display help for command
```

* * *

### migrate rtl

The `rtl` migration transforms your components to support RTL (right-to-left) languages.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest migrate rtl

Copy
```

This will:

1. Update `components.json` to set `rtl: true`
2. Transform physical CSS properties to logical equivalents (e.g., `ml-4` → `ms-4`, `text-left` → `text-start`)
3. Add `rtl:` variants where needed (e.g., `space-x-4` → `space-x-4 rtl:space-x-reverse`)

**Migrate specific files**

You can migrate specific files or use glob patterns:

```
Copy# Migrate a specific file
npx shadcn@latest migrate rtl src/components/ui/button.tsx

# Migrate files matching a glob pattern
npx shadcn@latest migrate rtl "src/components/ui/**"
```

If no path is provided, the migration will transform all files in your `ui` directory (from `components.json`).

* * *

### migrate radix

The `radix` migration updates your imports from individual `@radix-ui/react-*` packages to the unified `radix-ui` package.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest migrate radix

Copy
```

This will:

1. Transform imports from `@radix-ui/react-*` to `radix-ui`
2. Add the `radix-ui` package to your `package.json`

**Before**

```
Copyimport * as DialogPrimitive from "@radix-ui/react-dialog"
import * as SelectPrimitive from "@radix-ui/react-select"
```

**After**

```
Copyimport { Dialog as DialogPrimitive, Select as SelectPrimitive } from "radix-ui"
```

**Migrate specific files**

You can migrate specific files or use glob patterns:

```
Copy# Migrate a specific file.
npx shadcn@latest migrate radix src/components/ui/dialog.tsx

# Migrate files matching a glob pattern.
npx shadcn@latest migrate radix "src/components/ui/**"
```

If no path is provided, the migration will transform all files in your `ui` directory (from `components.json`).

Once complete, you can remove any unused `@radix-ui/react-*` packages from your `package.json`.

[RTL](/docs/rtl) [Monorepo](/docs/monorepo)

On This Page

[init](#init) [add](#add) [view](#view) [search](#search) [build](#build) [docs](#docs) [info](#info) [migrate](#migrate) [migrate rtl](#migrate-rtl) [migrate radix](#migrate-radix)

Deploy your shadcn/ui app on Vercel

Trusted by OpenAI, Sonos, Adobe, and more.

Vercel provides tools and infrastructure to deploy apps and features at scale.

Deploy Now [Deploy to Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout)

Built by [shadcn](https://twitter.com/shadcn) at [Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout). The source code is available on [GitHub](https://github.com/shadcn-ui/ui).

Create Project