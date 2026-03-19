package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var removeCmd = &cobra.Command{
	Use:   "remove [name]",
	Short: "Remove a QuanuX extension (Alias for 'ext remove')",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("remove [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(removeCmd)
	removeCmd.Flags().BoolP("force", "f", false, "")
	viper.BindPFlag("remove.remove.force", removeCmd.Flags().Lookup("force"))
}

