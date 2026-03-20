package cmd

import (
	"github.com/QuanuX/qxctl/pkg/lifecycle"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var lifecycleCmd = &cobra.Command{
	Use:   "lifecycle",
	Short: "Dynamic Habitat Deployment & Telemetry Lifecycle",
}

var lifecycleDeployCmd = &cobra.Command{
	Use:   "deploy",
	Short: "Deploys a payload into the outer shell (Habitat) safely using valid wiring hooks",
	RunE: func(cmd *cobra.Command, args []string) error {
		payload := viper.GetString("lifecycle.lifecycle.deploy.payload")
		target := viper.GetString("lifecycle.lifecycle.deploy.target")
		pType := viper.GetString("lifecycle.lifecycle.deploy.type")
		return lifecycle.Deploy(cmd.Context(), payload, target, pType)
	},
}

var lifecyclePredeployCmd = &cobra.Command{
	Use:   "predeploy",
	Short: "Performs a Capability Handshake prior to deployment",
}

var lifecycleSshCmd = &cobra.Command{
	Use:   "ssh [target] [payload_name]",
	Short: "Executes a payload via SSH, conditioned on the C++ Envoy generating the wrapper",
}

func init() {
	rootCmd.AddCommand(lifecycleCmd)
	lifecycleCmd.AddCommand(lifecycleDeployCmd)
	lifecycleDeployCmd.Flags().StringP("payload", "p", "", "Path to payload directory")
	viper.BindPFlag("lifecycle.lifecycle.deploy.payload", lifecycleDeployCmd.Flags().Lookup("payload"))
	lifecycleDeployCmd.Flags().StringP("target", "t", "", "Target Node Hostname")
	viper.BindPFlag("lifecycle.lifecycle.deploy.target", lifecycleDeployCmd.Flags().Lookup("target"))
	lifecycleDeployCmd.Flags().String("type", "extension", "Payload Type")
	viper.BindPFlag("lifecycle.lifecycle.deploy.type", lifecycleDeployCmd.Flags().Lookup("type"))
	lifecycleCmd.AddCommand(lifecyclePredeployCmd)
	lifecyclePredeployCmd.Flags().StringP("payload", "p", "", "Path to payload directory")
	viper.BindPFlag("lifecycle.lifecycle.predeploy.payload", lifecyclePredeployCmd.Flags().Lookup("payload"))
	lifecyclePredeployCmd.Flags().StringP("target", "t", "", "Target Node Hostname")
	viper.BindPFlag("lifecycle.lifecycle.predeploy.target", lifecyclePredeployCmd.Flags().Lookup("target"))
	lifecyclePredeployCmd.Flags().String("type", "extension", "Payload Type")
	viper.BindPFlag("lifecycle.lifecycle.predeploy.type", lifecyclePredeployCmd.Flags().Lookup("type"))
	lifecycleCmd.AddCommand(lifecycleSshCmd)
}
