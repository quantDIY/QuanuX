package cmd

import (
	"github.com/QuanuX/qxctl/pkg/nest"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var nestCmd = &cobra.Command{
	Use:   "nest",
	Short: "Manage QuanuX C++ Sovereign Engine Nest Deployment",
}

var nestDropCmd = &cobra.Command{
	Use:   "drop [target]",
	Short: "Deploys the C++ execution logic into a pre-conditioned OS Habitat",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		engine := viper.GetString("nest.nest.drop.engine")
		return nest.Drop(cmd.Context(), args[0], engine)
	},
}

func init() {
	rootCmd.AddCommand(nestCmd)
	nestCmd.AddCommand(nestDropCmd)
	nestDropCmd.Flags().String("engine", "spreader", "Engine type to compile and bind")
	viper.BindPFlag("nest.nest.drop.engine", nestDropCmd.Flags().Lookup("engine"))
}
