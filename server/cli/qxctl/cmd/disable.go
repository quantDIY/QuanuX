package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewDisableCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "disable [feature]",
		Short: "Disable advanced functionality toggles",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "disable", args[0])
		},
	}
	return cmd
}
