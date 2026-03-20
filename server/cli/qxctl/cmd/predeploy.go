package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/manager"
	"github.com/spf13/cobra"
)

func NewPredeployCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "predeploy",
		Short: "Performs a Capability Handshake prior to deployment natively",
		RunE: func(cmd *cobra.Command, args []string) error {
			payload, _ := cmd.Flags().GetString("payload")
			return manager.Exec(app.Ctx, "predeploy", payload)
		},
	}
	cmd.Flags().StringP("payload", "p", "", "Execution context payload")
	cmd.Flags().StringP("target", "t", "", "Node target array")
	cmd.Flags().String("type", "extension", "Deployment bounds mode")

	return cmd
}
