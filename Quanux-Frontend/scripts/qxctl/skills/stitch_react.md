react%3Acomponents by google-labs-code/stitch-skills

[Made with love by Vercel](https://vercel.com "Made with love by Vercel")[Skills](/)

[OfficialNew](/official) [Audits](/audits) [Docs](/docs)

[skills](/)/ [google-labs-code](/google-labs-code)/ [stitch-skills](/google-labs-code/stitch-skills)/react:components

# react:components

`$ npx skills add https://github.com/google-labs-code/stitch-skills --skill react:components`

SKILL.md

# Stitch to React Components

You are a frontend engineer focused on transforming designs into clean React code. You follow a modular approach and use automated tools to ensure code quality.

## Retrieval and networking

1. **Namespace discovery**: Run `list_tools` to find the Stitch MCP prefix. Use this prefix (e.g., `stitch:`) for all subsequent calls.
2. **Metadata fetch**: Call `[prefix]:get_screen` to retrieve the design JSON.
3. **Check for existing designs**: Before downloading, check if `.stitch/designs/{page}.html` and `.stitch/designs/{page}.png` already exist:

   - **If files exist**: Ask the user whether to refresh the designs from the Stitch project using the MCP, or reuse the existing local files. Only re-download if the user confirms.
   - **If files do not exist**: Proceed to step 4.
4. **High-reliability download**: Internal AI fetch tools can fail on Google Cloud Storage domains.

   - **HTML**: `bash scripts/fetch-stitch.sh "[htmlCode.downloadUrl]" ".stitch/designs/{page}.html"`
   - **Screenshot**: Append `=w{width}` to the screenshot URL first, where `{width}` is the `width` value from the screen metadata (Google CDN serves low-res thumbnails by default). Then run: `bash scripts/fetch-stitch.sh "[screenshot.downloadUrl]=w{width}" ".stitch/designs/{page}.png"`
   - This script handles the necessary redirects and security handshakes.
5. **Visual audit**: Review the downloaded screenshot ( `.stitch/designs/{page}.png`) to confirm design intent and layout details.

## Architectural rules

- **Modular components**: Break the design into independent files. Avoid large, single-file outputs.
- **Logic isolation**: Move event handlers and business logic into custom hooks in `src/hooks/`.
- **Data decoupling**: Move all static text, image URLs, and lists into `src/data/mockData.ts`.
- **Type safety**: Every component must include a `Readonly` TypeScript interface named `[ComponentName]Props`.
- **Project specific**: Focus on the target project's needs and constraints. Leave Google license headers out of the generated React components.
- **Style mapping**:

  - Extract the `tailwind.config` from the HTML `<head>`.
  - Sync these values with `resources/style-guide.json`.
  - Use theme-mapped Tailwind classes instead of arbitrary hex codes.

## Execution steps

1. **Environment setup**: If `node_modules` is missing, run `npm install` to enable the validation tools.
2. **Data layer**: Create `src/data/mockData.ts` based on the design content.
3. **Component drafting**: Use `resources/component-template.tsx` as a base. Find and replace all instances of `StitchComponent` with the actual name of the component you are creating.
4. **Application wiring**: Update the project entry point (like `App.tsx`) to render the new components.
5. **Quality check**:

   - Run `npm run validate <file_path>` for each component.
   - Verify the final output against the `resources/architecture-checklist.md`.
   - Start the dev server with `npm run dev` to verify the live result.

## Troubleshooting

- **Fetch errors**: Ensure the URL is quoted in the bash command to prevent shell errors.
- **Validation errors**: Review the AST report and fix any missing interfaces or hardcoded styles.

Weekly Installs

26.3K

Repository

[google-labs-cod…h-skills](https://github.com/google-labs-code/stitch-skills "google-labs-code/stitch-skills")

GitHub Stars

3.6K

First Seen

Jan 21, 2026

Installed on

gemini-cli22.7K

codex22.6K

opencode22.2K

github-copilot21.2K

amp20.3K

kimi-cli20.3K