package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var enableCmd = &cobra.Command{
	Use:   "enable [feature]",
	Short: "Enable advanced functionality toggles",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("enable [feature] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(enableCmd)
}

