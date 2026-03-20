package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewRemoveCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "remove [name]",
		Short: "Remove a QuanuX extension (Alias for 'ext remove')",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "remove", args[0])
		},
	}
	cmd.Flags().BoolP("force", "f", false, "Force delete target")
	return cmd
}
