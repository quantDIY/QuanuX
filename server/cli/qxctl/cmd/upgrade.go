package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

var upgradeCmd = &cobra.Command{
	Use:   "upgrade [name]",
	Short: "Auto-upgrade extension (Alias for 'ext upgrade')",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "upgrade", args[0])
	},
}

func init() {
	rootCmd.AddCommand(upgradeCmd)
}
