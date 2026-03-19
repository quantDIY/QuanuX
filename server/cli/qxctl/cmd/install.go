package cmd

import (
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var installCmd = &cobra.Command{
	Use:   "install [name]",
	Short: "Install a QuanuX extension (Alias for 'ext install')",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return manager.Exec(cmd.Context(), "install", args[0])
	},
}

func init() {
	rootCmd.AddCommand(installCmd)
	installCmd.Flags().StringP("version", "v", "", "")
	viper.BindPFlag("install.install.version", installCmd.Flags().Lookup("version"))
}
