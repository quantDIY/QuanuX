React Router - shadcn/ui

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

# React Router

Copy Page

[Previous](/docs/installation/laravel) [Next](/docs/installation/remix)

Install and configure shadcn/ui for React Router.

Choose the setup that matches your starting point.

[Use shadcn/create\
\
Build your preset and generate a React Router project.](#scaffold-with-create) [Use the CLI\
\
Scaffold a new React Router project directly from the terminal.](#scaffold-with-cli) [Existing Project\
\
Configure shadcn/ui manually in an existing React Router project.](#existing-react-router-project)

## Use shadcn/create

### Build Your Preset

Open [shadcn/create](/create?template=react-router) and build your preset visually. Choose your style, colors, fonts, icons, and more.

[Open shadcn/create](/create?template=react-router)

### Create Project

Click `Create Project`, choose your package manager, and copy the generated command.

The generated command will look similar to this:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest init --preset [CODE] --template react-router

Copy
```

The exact command will include your selected options such as `--base`, `--monorepo`, or `--rtl`.

### Add Components

Add the `Card` component to your project:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add card

Copy
```

If you created a monorepo, run the command from `apps/web` or specify the workspace from the repo root:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add card -c apps/web

Copy
```

The command above will add the `Card` component to your project. You can then import it like this:

app/routes/home.tsx

```
Copyimport {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "~/components/ui/card"

export default function Home() {
  return (
    <Card className="max-w-sm">
      <CardHeader>
        <CardTitle>Project Overview</CardTitle>
        <CardDescription>
          Track progress and recent activity for your React Router app.
        </CardDescription>
      </CardHeader>
      <CardContent>
        Your design system is ready. Start building your next component.
      </CardContent>
    </Card>
  )
}
```

If you created a monorepo, update `apps/web/app/routes/home.tsx` and import from `@workspace/ui/components/card` instead.

## Use the CLI

### Create Project

Run the `init` command to scaffold a new React Router project. Follow the prompts to configure your project: base, preset, monorepo, and more.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest init -t react-router

Copy
```

**For a monorepo project, use `--monorepo` flag:**

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest init -t react-router --monorepo

Copy
```

### Add Components

Add the `Card` component to your project:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add card

Copy
```

If you created a monorepo, run the command from `apps/web` or specify the workspace from the repo root:

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add card -c apps/web

Copy
```

The command above will add the `Card` component to your project. You can then import it like this:

app/routes/home.tsx

```
Copyimport {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "~/components/ui/card"

export default function Home() {
  return (
    <Card className="max-w-sm">
      <CardHeader>
        <CardTitle>Project Overview</CardTitle>
        <CardDescription>
          Track progress and recent activity for your React Router app.
        </CardDescription>
      </CardHeader>
      <CardContent>
        Your design system is ready. Start building your next component.
      </CardContent>
    </Card>
  )
}
```

If you created a monorepo, update `apps/web/app/routes/home.tsx` and import from `@workspace/ui/components/card` instead.

## Existing Project

### Create Project

If you need a new React Router project, create one first. Otherwise, skip this step.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm create react-router@latest

Copy
```

`create-react-router` already configures Tailwind CSS and the default `~/*` import alias for you. If you're adding shadcn/ui to an older or custom React Router app, make sure both are configured before continuing.

### Run the CLI

Run the `shadcn` init command to set up shadcn/ui in your project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest init

Copy
```

### Add Components

You can now start adding components to your project.

```relative font-mono text-sm leading-none

pnpmnpmyarnbun

pnpm dlx shadcn@latest add button

Copy
```

The command above will add the `Button` component to your project. You can then import it like this:

app/routes/home.tsx

```
Copyimport { Button } from "~/components/ui/button"

export default function Home() {
  return (
    <div className="flex min-h-svh flex-col items-center justify-center">
      <Button>Click me</Button>
    </div>
  )
}
```

[Laravel](/docs/installation/laravel) [Remix](/docs/installation/remix)

On This Page

[Use shadcn/create](#use-shadcncreate) [Build Your Preset](#build-your-preset) [Create Project](#create-project) [Add Components](#add-components) [Use the CLI](#use-the-cli) [Create Project](#create-project-1) [Add Components](#add-components-1) [Existing Project](#existing-project) [Create Project](#create-project-2) [Run the CLI](#run-the-cli) [Add Components](#add-components-2)

Deploy your shadcn/ui app on Vercel

Trusted by OpenAI, Sonos, Adobe, and more.

Vercel provides tools and infrastructure to deploy apps and features at scale.

Deploy Now [Deploy to Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout)

Built by [shadcn](https://twitter.com/shadcn) at [Vercel](https://vercel.com/new?utm_source=shadcn_site&utm_medium=web&utm_campaign=docs_cta_deploy_now_callout). The source code is available on [GitHub](https://github.com/shadcn-ui/ui).

Create Project