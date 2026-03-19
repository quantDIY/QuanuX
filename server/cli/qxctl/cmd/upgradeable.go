package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

var upgradeableCmd = &cobra.Command{
	Use:   "upgradeable [name]",
	Short: "Check for updates (Alias for 'ext upgradeable')",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "upgradeable", args[0])
	},
}

func init() {
	rootCmd.AddCommand(upgradeableCmd)
}
