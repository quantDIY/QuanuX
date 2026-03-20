package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/indicators"
	"github.com/spf13/cobra"
)

func NewIndicatorsCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "indicators",
		Short: "Manage Indicator Registry (Community)",
	}

	installCmd := &cobra.Command{
		Use:   "install [url]",
		Short: "Install a community indicator from Git",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			name, _ := cmd.Flags().GetString("name")
			return indicators.Install(app.Ctx, args[0], name)
		},
	}
	installCmd.Flags().String("name", "", "Local name for the indicator package")

	listCmd := &cobra.Command{
		Use:   "list", Short: "List installed community indicators", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	removeCmd := &cobra.Command{
		Use:   "remove [name]", Short: "Remove a community indicator", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(installCmd, listCmd, removeCmd)
	return cmd
}
