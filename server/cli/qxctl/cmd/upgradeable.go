package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var upgradeableCmd = &cobra.Command{
	Use:   "upgradeable [name]",
	Short: "Check for updates (Alias for 'ext upgradeable')",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("upgradeable [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(upgradeableCmd)
}

