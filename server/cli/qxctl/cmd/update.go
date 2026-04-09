package cmd

import (
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

func NewUpdateCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "update",
		Short: "Update system definitions, core scripts, or skill catalog registries",
	}

	frontendSkillsCmd := &cobra.Command{
		Use:   "frontend-skills",
		Short: "Dynamically audits frontend libraries for Basic Tenet compliance and catalogs Agent OS skills via an Obsidian Vault",
		RunE: func(cmd *cobra.Command, args []string) error {
			app.Out.Info("Initiating canonical frontend skill audit & update process...")
			
			workspaceRoot := os.Getenv("QUANUX_WORKSPACE")
			if workspaceRoot == "" {
				workspaceRoot = "/Users/Duncan/Antigravity/QuanuX/QuanuX"
			}
			frontendDir := filepath.Join(workspaceRoot, "Quanux-Frontend")
			targetDir := filepath.Join(frontendDir, "scripts", "qxctl")
			
			if err := os.MkdirAll(targetDir, 0755); err != nil {
				return fmt.Errorf("failed to create script target payload directory: %w", err)
			}

			app.Out.Info("Enforcing architectural presentation bounds across /packages/ui (excluding Providers/Hooks)...")
			err := filepath.Walk(filepath.Join(frontendDir, "packages", "ui"), func(path string, info os.FileInfo, err error) error {
				if err != nil || info.IsDir() {
					return nil
				}
				
				// Exclude whitelisted directories from Beast Mode enforcement
				if strings.Contains(path, "providers") || strings.Contains(path, "hooks") {
					return nil
				}

				if filepath.Ext(path) == ".tsx" || filepath.Ext(path) == ".ts" {
					content, _ := os.ReadFile(path)
					contentStr := string(content)
					
					if regexp.MustCompile(`\[\s*\{\s*id:`).MatchString(contentStr) {
						app.Out.Warn(fmt.Sprintf("VIOLATION [Rule 1]: Hardcoded mock arrays detected in client component: %s", info.Name()))
					}

					if regexp.MustCompile(`\buseState\b`).MatchString(contentStr) && !regexp.MustCompile(`useBeastModeBuffer`).MatchString(contentStr) {
						app.Out.Warn(fmt.Sprintf("VIOLATION [Rule 2]: File %s utilizes standard local state. Wrap with useBeastModeBuffer.", info.Name()))
					}
				}
				return nil
			})
			if err != nil {
				app.Out.Warn("Could not crawl frontend directory, skipping compliance check.")
			}

			vaultRef, _ := cmd.Flags().GetString("vault")
			if vaultRef == "" {
				app.Out.Info("No --vault specified. Skipping remote skill registry update. Provide a vault path or URL to configure.")
				return nil
			}

			app.Out.Info(fmt.Sprintf("Configuring canonical skill registry to reference Obsidian Vault at: %s", vaultRef))

			// Generate canonical index.yaml with external vault reference
			yamlOut := filepath.Join(targetDir, "index.yaml")
			
			var sb strings.Builder
			sb.WriteString("# QuanuX Canonical Skill Index\n")
			sb.WriteString(fmt.Sprintf("vault_reference: %q\n\n", vaultRef))
			sb.WriteString("skills:\n")
			
			// Inject strictly local components first
			sb.WriteString("  - id: \"quanux.frontend.beast_mode\"\n    skill_name: \"beast-mode-foundation\"\n    path: \"/QuanuX/Quanux-Frontend/packages/ui/src/hooks/useBeastModeBuffer.ts\"\n    status: \"active\"\n    entry_priority: 1\n\n")

			if err := os.WriteFile(yamlOut, []byte(sb.String()), 0644); err != nil {
				return fmt.Errorf("failed to write canonical schema index.yaml: %w", err)
			}
			
			app.Out.Info("Successfully enforced strictly governed UI Tenets and updated skill registry reference.")
			return nil
		},
	}

	frontendSkillsCmd.Flags().String("vault", "", "Path or URL to the Obsidian Skills Vault")

	cmd.AddCommand(frontendSkillsCmd)
	return cmd
}
