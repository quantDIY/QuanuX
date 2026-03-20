package cmd

import (
	"github.com/QuanuX/qxctl/pkg/node"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var nodeCmd = &cobra.Command{
	Use:   "node",
	Short: "Manage Remote Execution Nodes (Deploy, List)",
}

var nodeDeployCmd = &cobra.Command{
	Use:   "deploy [target]",
	Short: "Deploy the QuanuX Execution Node to a remote server via SSH",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		target := args[0]
		hub := viper.GetString("node.node.deploy.hub")
		token := viper.GetString("node.node.deploy.token")
		dryRun := viper.GetBool("node.node.deploy.dry_run")
		return node.Deploy(cmd.Context(), target, hub, token, dryRun)
	},
}

func init() {
	rootCmd.AddCommand(nodeCmd)
	nodeCmd.AddCommand(nodeDeployCmd)
	nodeDeployCmd.Flags().String("hub", "", "NATS Hub URL (e.g. nats://hub.quanux.io:4222)")
	viper.BindPFlag("node.node.deploy.hub", nodeDeployCmd.Flags().Lookup("hub"))
	nodeDeployCmd.Flags().String("token", "", "Registration Token")
	viper.BindPFlag("node.node.deploy.token", nodeDeployCmd.Flags().Lookup("token"))
	nodeDeployCmd.Flags().Bool("dry-run", false, "Print command without executing")
	viper.BindPFlag("node.node.deploy.dry_run", nodeDeployCmd.Flags().Lookup("dry-run"))
}
