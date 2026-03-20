package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewInstallCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "install [name]",
		Short: "Install a QuanuX extension (Alias for 'ext install')",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return manager.Exec(app.Ctx, "install", args[0])
		},
	}
	cmd.Flags().StringP("version", "v", "", "Version block")
	return cmd
}
