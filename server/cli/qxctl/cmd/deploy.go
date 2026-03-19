package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
)

var rootDeployCmd = &cobra.Command{
	Use:   "deploy",
	Short: "Dynamic Habitat Deployment & Telemetry Lifecycle.",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("deploy module initialized. Tauri JSON-RPC listener ready.")
	},
}

func init() {
	rootCmd.AddCommand(rootDeployCmd)
}
