package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var disableCmd = &cobra.Command{
	Use:   "disable [feature]",
	Short: "Disable advanced functionality toggles",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("disable [feature] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(disableCmd)
}

