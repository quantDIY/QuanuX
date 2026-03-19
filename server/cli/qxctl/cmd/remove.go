package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var removeCmd = &cobra.Command{
	Use:   "remove [name]",
	Short: "Remove a QuanuX extension (Alias for 'ext remove')",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "remove", args[0])
	},
}

func init() {
	rootCmd.AddCommand(removeCmd)
	removeCmd.Flags().BoolP("force", "f", false, "")
	viper.BindPFlag("remove.remove.force", removeCmd.Flags().Lookup("force"))
}
