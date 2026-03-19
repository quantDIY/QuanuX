package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

var disableCmd = &cobra.Command{
	Use:   "disable [feature]",
	Short: "Disable advanced functionality toggles",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "disable", args[0])
	},
}

func init() {
	rootCmd.AddCommand(disableCmd)
}
