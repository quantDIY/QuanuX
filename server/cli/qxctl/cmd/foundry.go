package cmd

import (
	"github.com/QuanuX/qxctl/pkg/foundry"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var foundryCmd = &cobra.Command{
	Use:   "foundry",
	Short: "Manage QuanuX Foundry (Strategy Generator)",
}

var foundryDeployCmd = &cobra.Command{
	Use:   "deploy",
	Short: "Vendor code and push a fully self-sovereign artifact to an Execution Node",
}

var foundryForgeCmd = &cobra.Command{
	Use:   "forge",
	Short: "Forge a new sovereign trading component via AI Generation",
	RunE: func(cmd *cobra.Command, args []string) error {
		cType := viper.GetString("foundry.foundry.forge.type")
		name := viper.GetString("foundry.foundry.forge.name")
		lang := viper.GetString("foundry.foundry.forge.lang")
		version := viper.GetString("foundry.foundry.forge.version")
		return foundry.Forge(cmd.Context(), cType, name, lang, version)
	},
}

var foundryInspectCmd = &cobra.Command{
	Use:   "inspect [target]",
	Short: "Connect to edge node for real-time inspection and modular hot-swapping",
}

var foundryVerifyCmd = &cobra.Command{
	Use:   "verify",
	Short: "Trigger the Deterministic Sandbox for mathematical equivalence testing",
}

func init() {
	rootCmd.AddCommand(foundryCmd)
	foundryCmd.AddCommand(foundryDeployCmd)
	foundryDeployCmd.Flags().StringP("target", "t", "", "Strategy variant to deploy")
	viper.BindPFlag("foundry.foundry.deploy.target", foundryDeployCmd.Flags().Lookup("target"))
	foundryDeployCmd.Flags().StringP("node", "n", "", "Remote Node ID mapping")
	viper.BindPFlag("foundry.foundry.deploy.node", foundryDeployCmd.Flags().Lookup("node"))
	foundryCmd.AddCommand(foundryForgeCmd)
	foundryForgeCmd.Flags().StringP("type", "t", "", "Type of component to forge (entry, exit, indicator, strategy)")
	viper.BindPFlag("foundry.foundry.forge.type", foundryForgeCmd.Flags().Lookup("type"))
	foundryForgeCmd.Flags().StringP("name", "n", "", "Name of the component")
	viper.BindPFlag("foundry.foundry.forge.name", foundryForgeCmd.Flags().Lookup("name"))
	foundryForgeCmd.Flags().StringP("lang", "l", "", "Target language constraint (python, cython, cpp)")
	viper.BindPFlag("foundry.foundry.forge.lang", foundryForgeCmd.Flags().Lookup("lang"))
	foundryForgeCmd.Flags().StringP("version", "v", "", "Language version constraint (e.g. 3.11, 3.14, cpp20)")
	viper.BindPFlag("foundry.foundry.forge.version", foundryForgeCmd.Flags().Lookup("version"))
	foundryCmd.AddCommand(foundryInspectCmd)
	foundryCmd.AddCommand(foundryVerifyCmd)
	foundryVerifyCmd.Flags().StringP("target", "t", "", "Name of the strategy/component to verify")
	viper.BindPFlag("foundry.foundry.verify.target", foundryVerifyCmd.Flags().Lookup("target"))
}
