package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/module"
	"github.com/spf13/cobra"
)

func NewModuleCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "module",
		Short: "Manage Core Modules (Lifecycle)",
	}

	checkCmd := &cobra.Command{
		Use:   "check [module]",
		Short: "Verify module integrity",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return module.Check(app.Ctx, "check", args[0])
		},
	}

	installCmd := &cobra.Command{
		Use:   "install [module]",
		Short: "Re-install a core module (Restore form)",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return module.Check(app.Ctx, "install", args[0])
		},
	}

	removeCmd := &cobra.Command{
		Use:   "remove [module]",
		Short: "Uninstall a core module (Clean Removal)",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return module.Check(app.Ctx, "remove", args[0])
		},
	}

	cmd.AddCommand(checkCmd, installCmd, removeCmd)
	return cmd
}
