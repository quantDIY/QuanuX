package cmd

import (
	"github.com/QuanuX/qxctl/pkg/spreader"
	"github.com/spf13/cobra"
)

var spreaderCmd = &cobra.Command{
	Use:   "spreader",
	Short: "Manage QuanuX-Spreader Engine Deployment and Strategy Injection",
}

var spreaderDeployCmd = &cobra.Command{
	Use:   "deploy [binary_name]",
	Short: "Push the compiled Spreader to the Execution Node stub, validating the Checksum Signature",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return spreader.Deploy(cmd.Context(), args[0])
	},
}

var spreaderPackageCmd = &cobra.Command{
	Use:   "package [strategy_json]",
	Short: "Transpile the strategy into C++ via Cython constructs and forge the 59ns QuanuX-Spreader binary",
}

func init() {
	rootCmd.AddCommand(spreaderCmd)
	spreaderCmd.AddCommand(spreaderDeployCmd)
	spreaderCmd.AddCommand(spreaderPackageCmd)
}
