package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewEnableCmd(app *runtime.App) *cobra.Command {
	return &cobra.Command{
		Use:   "enable [feature]",
		Short: "Enable advanced functionality toggles natively",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "enable", args[0])
		},
	}
}
