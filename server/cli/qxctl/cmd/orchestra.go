package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/orchestra"
	"github.com/spf13/cobra"
)

func NewOrchestraCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "orchestra",
		Short: "Manage QuanuX-Orchestra Universal Naming Registry",
	}

	bootstrapCmd := &cobra.Command{
		Use:   "bootstrap",
		Short: "Executes the Python script to download the ISO 20022 XML and generate constants",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	compileCmd := &cobra.Command{
		Use:   "compile",
		Short: "Executes standardizer_cli, generating the C++ constexpr bridge, the Cython wrappers, and embedding the checksums",
		RunE: func(cmd *cobra.Command, args []string) error {
			venue, _ := cmd.Flags().GetString("venue")
			return orchestra.Compile(app.Ctx, venue)
		},
	}
	compileCmd.Flags().StringP("venue", "v", "", "The broker venue to compile (e.g. ibkr)")

	verifyCmd := &cobra.Command{
		Use:   "verify", Short: "Checks parity between active checksums safely", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(bootstrapCmd, compileCmd, verifyCmd)
	return cmd
}
