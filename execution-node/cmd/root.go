package cmd

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
)

var rootCmd = &cobra.Command{
	Use:   "quanux-node",
	Short: "QuanuX High-Performance Execution Engine",
	Long: `The specific Execution Node for the QuanuX Platform.
It serves as a supervisor for high-performance trading strategies and 
manages the connection to the QuanuX Hub.`,
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
