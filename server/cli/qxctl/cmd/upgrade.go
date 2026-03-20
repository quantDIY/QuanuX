package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewUpgradeCmd(app *runtime.App) *cobra.Command {
	return &cobra.Command{
		Use:   "upgrade [name]",
		Short: "Auto-upgrade extension (Alias for 'ext upgrade')",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "upgrade", args[0])
		},
	}
}
