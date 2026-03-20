package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/secrets"
	"github.com/spf13/cobra"
)

func NewSecretsCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "secrets",
		Short: "Manage API keys and secrets via OS Keyring natively",
	}

	getCmd := &cobra.Command{
		Use:   "get [key]",
		Short: "Retrieve and print a secret value safely",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	listCmd := &cobra.Command{
		Use:   "list-keys",
		Short: "List configured keys (hiding values)",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	setCmd := &cobra.Command{
		Use:   "set [key] [value]",
		Short: "Set a specific secret key safely directly into Keyring targets",
		Args:  cobra.ExactArgs(2),
		RunE: func(cmd *cobra.Command, args []string) error {
			return secrets.Set(app.Ctx, args[0], args[1])
		},
	}

	setupCmd := &cobra.Command{
		Use:   "setup", Short: "Interactive wizard safely bound to outputs", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(getCmd, listCmd, setCmd, setupCmd)
	return cmd
}
