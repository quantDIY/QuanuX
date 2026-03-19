package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var secretsCmd = &cobra.Command{
	Use:   "secrets",
	Short: "Manage API keys and secrets via OS Keyring",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("secrets invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var secretsGetCmd = &cobra.Command{
	Use:   "get [key]",
	Short: "Retrieve and print a secret value (Show plaintext)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("get [key] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var secretsListKeysCmd = &cobra.Command{
	Use:   "list-keys",
	Short: "List configured keys (hiding values)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("list-keys invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var secretsSetCmd = &cobra.Command{
	Use:   "set [key] [value]",
	Short: "Set a specific secret key",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("set [key] [value] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var secretsSetupCmd = &cobra.Command{
	Use:   "setup",
	Short: "Interactive wizard to set up common API keys",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("setup invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(secretsCmd)
	secretsCmd.AddCommand(secretsGetCmd)
	secretsCmd.AddCommand(secretsListKeysCmd)
	secretsCmd.AddCommand(secretsSetCmd)
	secretsCmd.AddCommand(secretsSetupCmd)
}

