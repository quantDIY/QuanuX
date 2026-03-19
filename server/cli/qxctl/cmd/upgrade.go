package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var upgradeCmd = &cobra.Command{
	Use:   "upgrade [name]",
	Short: "Auto-upgrade extension (Alias for 'ext upgrade')",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("upgrade [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(upgradeCmd)
}

