package cmd

import (
	"fmt"
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
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("check [module] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var moduleInstallCmd = &cobra.Command{
	Use:   "install [module]",
	Short: "Re-install a core module (Restore form)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("install [module] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var moduleRemoveCmd = &cobra.Command{
	Use:   "remove [module]",
	Short: "Uninstall a core module (Clean Removal)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("remove [module] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(moduleCmd)
	moduleCmd.AddCommand(moduleCheckCmd)
	moduleCmd.AddCommand(moduleInstallCmd)
	moduleCmd.AddCommand(moduleRemoveCmd)
}

