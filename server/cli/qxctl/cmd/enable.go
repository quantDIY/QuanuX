package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

var enableCmd = &cobra.Command{
	Use:   "enable [feature]",
	Short: "Enable advanced functionality toggles",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "enable", args[0])
	},
}

func init() {
	rootCmd.AddCommand(enableCmd)
}
