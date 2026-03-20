package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/node"
	"github.com/spf13/cobra"
)

func NewNodeCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "node",
		Short: "Manage Remote Execution Nodes (Deploy, List)",
	}

	deployCmd := &cobra.Command{
		Use:   "deploy [target]",
		Short: "Deploy the QuanuX Execution Node to a remote server via SSH",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			hub, _ := cmd.Flags().GetString("hub")
			if hub == "" {
				hub = app.Cfg.HubURL
			}
			token, _ := cmd.Flags().GetString("token")
			dryRun, _ := cmd.Flags().GetBool("dry-run")
			return node.Deploy(app.Ctx, args[0], hub, token, dryRun)
		},
	}
	deployCmd.Flags().String("hub", "", "NATS Hub URL (Falls back to app trajectory by default)")
	deployCmd.Flags().String("token", "", "Registration Token")
	deployCmd.Flags().Bool("dry-run", false, "Print command without executing")

	cmd.AddCommand(deployCmd)
	return cmd
}
