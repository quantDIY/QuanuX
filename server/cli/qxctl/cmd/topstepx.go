package cmd

import (
	"fmt"

	"github.com/QuanuX/qxctl/pkg/topstepx"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var topstepxCmd = &cobra.Command{
	Use:   "topstepx",
	Short: "Manage TopstepX Extension",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("topstepx invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
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
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("env invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxInstallCmd = &cobra.Command{
	Use:   "install",
	Short: "Install TopstepX extension dependencies",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("install invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxMarketHubCmd = &cobra.Command{
	Use:   "market-hub [url]",
	Short: "Set SignalR Market Hub URL",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("market-hub [url] invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxPasswordCmd = &cobra.Command{
	Use:   "password [password]",
	Short: "Set TopstepX Password",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("password [password] invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxRemoveCmd = &cobra.Command{
	Use:   "remove",
	Short: "Remove TopstepX extension artifacts",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("remove invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxUserCmd = &cobra.Command{
	Use:   "user [username]",
	Short: "Set TopstepX Username",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("user [username] invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var topstepxUserHubCmd = &cobra.Command{
	Use:   "user-hub [url]",
	Short: "Set SignalR User Hub URL",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("user-hub [url] invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
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
