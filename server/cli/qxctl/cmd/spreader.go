package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var spreaderCmd = &cobra.Command{
	Use:   "spreader",
	Short: "Manage QuanuX-Spreader Engine Deployment and Strategy Injection",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("spreader invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var spreaderDeployCmd = &cobra.Command{
	Use:   "deploy [binary_name]",
	Short: "Push the compiled Spreader to the Execution Node stub, validating the Checksum Signature",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("deploy [binary_name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var spreaderPackageCmd = &cobra.Command{
	Use:   "package [strategy_json]",
	Short: "Transpile the strategy into C++ via Cython constructs and forge the 59ns QuanuX-Spreader binary",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("package [strategy_json] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(spreaderCmd)
	spreaderCmd.AddCommand(spreaderDeployCmd)
	spreaderCmd.AddCommand(spreaderPackageCmd)
}

