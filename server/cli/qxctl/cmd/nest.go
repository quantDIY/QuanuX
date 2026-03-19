package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var nestCmd = &cobra.Command{
	Use:   "nest",
	Short: "Manage QuanuX C++ Sovereign Engine Nest Deployment",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("nest invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var nestDropCmd = &cobra.Command{
	Use:   "drop [target]",
	Short: "Deploys the C++ execution logic into a pre-conditioned OS Habitat",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("drop [target] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(nestCmd)
	nestCmd.AddCommand(nestDropCmd)
	nestDropCmd.Flags().String("engine", "spreader", "Engine type to compile and bind")
	viper.BindPFlag("nest.nest.drop.engine", nestDropCmd.Flags().Lookup("engine"))
}

