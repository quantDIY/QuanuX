package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewUpgradeableCmd(app *runtime.App) *cobra.Command {
	return &cobra.Command{
		Use:   "upgradeable [name]",
		Short: "Check for updates (Alias for 'ext upgradeable')",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "upgradeable", args[0])
		},
	}
}
