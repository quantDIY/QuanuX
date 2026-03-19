package cmd

import (
	"fmt"

	"github.com/QuanuX/qxctl/pkg/orchestra"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var orchestraCmd = &cobra.Command{
	Use:   "orchestra",
	Short: "Manage QuanuX-Orchestra Universal Naming Registry",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("orchestra invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var orchestraBootstrapCmd = &cobra.Command{
	Use:   "bootstrap",
	Short: "Executes the Python script to download the ISO 20022 XML and generate constants",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("bootstrap invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var orchestraCompileCmd = &cobra.Command{
	Use:   "compile",
	Short: "Executes standardizer_cli, generating the C++ constexpr bridge, the Cython wrappers, and embedding the checksums",
	RunE: func(cmd *cobra.Command, args []string) error {
		venue := viper.GetString("orchestra.orchestra.compile.venue")
		return orchestra.Compile(cmd.Context(), venue)
	},
}

var orchestraVerifyCmd = &cobra.Command{
	Use:   "verify",
	Short: "Checks parity between the active Spreader binary checksum and the Python superGraph bindings",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("verify invoked.")
		fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(orchestraCmd)
	orchestraCmd.AddCommand(orchestraBootstrapCmd)
	orchestraCmd.AddCommand(orchestraCompileCmd)
	orchestraCompileCmd.Flags().StringP("venue", "v", "", "The broker venue to compile (e.g. ibkr)")
	viper.BindPFlag("orchestra.orchestra.compile.venue", orchestraCompileCmd.Flags().Lookup("venue"))
	orchestraCmd.AddCommand(orchestraVerifyCmd)
}
