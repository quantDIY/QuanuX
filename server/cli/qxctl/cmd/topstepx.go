package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/topstepx"
	"github.com/spf13/cobra"
)

func NewTopstepxCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "topstepx",
		Short: "Manage TopstepX Extension natively",
	}

	apikeyCmd := &cobra.Command{
		Use:   "apikey [key]",
		Short: "Set TopstepX API Key",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return topstepx.SetApiKey(app.Ctx, args[0])
		},
	}

	envCmd := &cobra.Command{Use: "env", Short: "Show TopstepX Environment Variables", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	installCmd := &cobra.Command{Use: "install", Short: "Install TopstepX extension dependencies", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	marketHubCmd := &cobra.Command{Use: "market-hub [url]", Short: "Set SignalR Market Hub URL", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	passwordCmd := &cobra.Command{Use: "password [password]", Short: "Set TopstepX Password", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	removeCmd := &cobra.Command{Use: "remove", Short: "Remove extension artifacts", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	userCmd := &cobra.Command{Use: "user [username]", Short: "Set Username", RunE: func(cmd *cobra.Command, args []string) error { return nil }}
	userHubCmd := &cobra.Command{Use: "user-hub [url]", Short: "Set User Hub URL", RunE: func(cmd *cobra.Command, args []string) error { return nil }}

	cmd.AddCommand(apikeyCmd, envCmd, installCmd, marketHubCmd, passwordCmd, removeCmd, userCmd, userHubCmd)
	return cmd
}
