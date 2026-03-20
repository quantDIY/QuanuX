package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/lifecycle"
	"github.com/spf13/cobra"
)

func NewLifecycleCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "lifecycle",
		Short: "Dynamic Habitat Deployment & Telemetry Lifecycle",
	}

	deployCmd := &cobra.Command{
		Use:   "deploy",
		Short: "Deploys a payload into the outer shell (Habitat) safely using valid wiring hooks",
		RunE: func(cmd *cobra.Command, args []string) error {
			payload, _ := cmd.Flags().GetString("payload")
			target, _ := cmd.Flags().GetString("target")
			pType, _ := cmd.Flags().GetString("type")
			return lifecycle.Deploy(app.Ctx, payload, target, pType)
		},
	}
	deployCmd.Flags().StringP("payload", "p", "", "Path to payload directory")
	deployCmd.Flags().StringP("target", "t", "", "Target Node Hostname")
	deployCmd.Flags().String("type", "extension", "Payload Type")

	predeployCmd := &cobra.Command{
		Use:   "predeploy", Short: "Performs a Capability Handshake prior to deployment", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	predeployCmd.Flags().StringP("payload", "p", "", "Path to payload directory")
	predeployCmd.Flags().StringP("target", "t", "", "Target Node Hostname")
	predeployCmd.Flags().String("type", "extension", "Payload Type")

	sshCmd := &cobra.Command{
		Use:   "ssh [target] [payload_name]", Short: "Executes a payload via SSH, conditioned on the C++ Envoy generating the wrapper", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(deployCmd, predeployCmd, sshCmd)
	return cmd
}
