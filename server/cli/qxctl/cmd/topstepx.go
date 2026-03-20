package cmd

import (
	"github.com/QuanuX/qxctl/pkg/topstepx"
	"github.com/spf13/cobra"
)

var topstepxCmd = &cobra.Command{
	Use:   "topstepx",
	Short: "Manage TopstepX Extension",
}

var topstepxApikeyCmd = &cobra.Command{
	Use:   "apikey [key]",
	Short: "Set TopstepX API Key",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return topstepx.SetApiKey(cmd.Context(), args[0])
	},
}

var topstepxEnvCmd = &cobra.Command{
	Use:   "env",
	Short: "Show TopstepX Environment Variables (secrets masked)",
}

var topstepxInstallCmd = &cobra.Command{
	Use:   "install",
	Short: "Install TopstepX extension dependencies",
}

var topstepxMarketHubCmd = &cobra.Command{
	Use:   "market-hub [url]",
	Short: "Set SignalR Market Hub URL",
}

var topstepxPasswordCmd = &cobra.Command{
	Use:   "password [password]",
	Short: "Set TopstepX Password",
}

var topstepxRemoveCmd = &cobra.Command{
	Use:   "remove",
	Short: "Remove TopstepX extension artifacts",
}

var topstepxUserCmd = &cobra.Command{
	Use:   "user [username]",
	Short: "Set TopstepX Username",
}

var topstepxUserHubCmd = &cobra.Command{
	Use:   "user-hub [url]",
	Short: "Set SignalR User Hub URL",
}

func init() {
	rootCmd.AddCommand(topstepxCmd)
	topstepxCmd.AddCommand(topstepxApikeyCmd)
	topstepxCmd.AddCommand(topstepxEnvCmd)
	topstepxCmd.AddCommand(topstepxInstallCmd)
	topstepxCmd.AddCommand(topstepxMarketHubCmd)
	topstepxCmd.AddCommand(topstepxPasswordCmd)
	topstepxCmd.AddCommand(topstepxRemoveCmd)
	topstepxCmd.AddCommand(topstepxUserCmd)
	topstepxCmd.AddCommand(topstepxUserHubCmd)
}
