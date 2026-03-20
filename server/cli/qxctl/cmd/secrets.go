package cmd

import (
	"github.com/QuanuX/qxctl/pkg/secrets"
	"github.com/spf13/cobra"
)

var secretsCmd = &cobra.Command{
	Use:   "secrets",
	Short: "Manage API keys and secrets via OS Keyring",
}

var secretsGetCmd = &cobra.Command{
	Use:   "get [key]",
	Short: "Retrieve and print a secret value (Show plaintext)",
}

var secretsListKeysCmd = &cobra.Command{
	Use:   "list-keys",
	Short: "List configured keys (hiding values)",
}

var secretsSetCmd = &cobra.Command{
	Use:   "set [key] [value]",
	Short: "Set a specific secret key",
	Args:  cobra.ExactArgs(2),
	RunE: func(cmd *cobra.Command, args []string) error {
		return secrets.Set(cmd.Context(), args[0], args[1])
	},
}

var secretsSetupCmd = &cobra.Command{
	Use:   "setup",
	Short: "Interactive wizard to set up common API keys",
}

func init() {
	rootCmd.AddCommand(secretsCmd)
	secretsCmd.AddCommand(secretsGetCmd)
	secretsCmd.AddCommand(secretsListKeysCmd)
	secretsCmd.AddCommand(secretsSetCmd)
	secretsCmd.AddCommand(secretsSetupCmd)
}
