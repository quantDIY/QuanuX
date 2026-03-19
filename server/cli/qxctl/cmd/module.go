package cmd

import (
	"fmt"

	"github.com/QuanuX/qxctl/pkg/module"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var moduleCmd = &cobra.Command{
	Use:   "module",
	Short: "Manage Core Modules (Lifecycle)",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("module invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var moduleCheckCmd = &cobra.Command{
	Use:   "check [module]",
	Short: "Verify module integrity",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return module.Check(cmd.Context(), "check", args[0])
	},
}

var moduleInstallCmd = &cobra.Command{
	Use:   "install [module]",
	Short: "Re-install a core module (Restore form)",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return module.Check(cmd.Context(), "install", args[0])
	},
}

var moduleRemoveCmd = &cobra.Command{
	Use:   "remove [module]",
	Short: "Uninstall a core module (Clean Removal)",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return module.Check(cmd.Context(), "remove", args[0])
	},
}

func init() {
	rootCmd.AddCommand(moduleCmd)
	moduleCmd.AddCommand(moduleCheckCmd)
	moduleCmd.AddCommand(moduleInstallCmd)
	moduleCmd.AddCommand(moduleRemoveCmd)
}
