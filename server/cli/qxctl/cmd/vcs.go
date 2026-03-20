package cmd

import (
	"github.com/QuanuX/qxctl/pkg/vcs"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var vcsCmd = &cobra.Command{
	Use:   "vcs",
	Short: "Version Control System Connectors",
}

var vcsCloneCmd = &cobra.Command{
	Use:   "clone [url]",
	Short: "Clone a remote repository",
}

var vcsCommitCmd = &cobra.Command{
	Use:   "commit",
	Short: "Commit changes to the local repository",
	RunE: func(cmd *cobra.Command, args []string) error {
		msg := viper.GetString("vcs.vcs.commit.message")
		all := viper.GetBool("vcs.vcs.commit.all")
		return vcs.Commit(cmd.Context(), msg, all)
	},
}

var vcsConnectCmd = &cobra.Command{
	Use:   "connect [url]",
	Short: "Connect current directory to a remote VCS",
}

var vcsPublishCmd = &cobra.Command{
	Use:   "publish [provider]",
	Short: "Create a remote repository on the provider and push the current project to it",
}

var vcsPushCmd = &cobra.Command{
	Use:   "push",
	Short: "Push changes to remote",
}

var vcsSetupCmd = &cobra.Command{
	Use:   "setup [provider]",
	Short: "Configure credentials for a VCS provider (GitHub, GitLab)",
}

var vcsStatusCmd = &cobra.Command{
	Use:   "status",
	Short: "Show status of the current repository",
	RunE: func(cmd *cobra.Command, args []string) error {
		return vcs.Status(cmd.Context())
	},
}

var vcsSyncCmd = &cobra.Command{
	Use:   "sync",
	Short: "Pull latest changes from the default remote (origin)",
}

func init() {
	rootCmd.AddCommand(vcsCmd)
	vcsCmd.AddCommand(vcsCloneCmd)
	vcsCloneCmd.Flags().String("target", "", "Target directory name")
	viper.BindPFlag("vcs.vcs.clone.target", vcsCloneCmd.Flags().Lookup("target"))
	vcsCmd.AddCommand(vcsCommitCmd)
	vcsCommitCmd.Flags().StringP("message", "m", "", "Commit message")
	viper.BindPFlag("vcs.vcs.commit.message", vcsCommitCmd.Flags().Lookup("message"))
	vcsCommitCmd.Flags().BoolP("all", "a", false, "Stage all modified files")
	viper.BindPFlag("vcs.vcs.commit.all", vcsCommitCmd.Flags().Lookup("all"))
	vcsCmd.AddCommand(vcsConnectCmd)
	vcsConnectCmd.Flags().String("name", "origin", "Remote name")
	viper.BindPFlag("vcs.vcs.connect.name", vcsConnectCmd.Flags().Lookup("name"))
	vcsCmd.AddCommand(vcsPublishCmd)
	vcsPublishCmd.Flags().String("name", "", "Repository name (defaults to current folder)")
	viper.BindPFlag("vcs.vcs.publish.name", vcsPublishCmd.Flags().Lookup("name"))
	vcsPublishCmd.Flags().Bool("private", true, "Create as private repository")
	viper.BindPFlag("vcs.vcs.publish.private", vcsPublishCmd.Flags().Lookup("private"))
	vcsCmd.AddCommand(vcsPushCmd)
	vcsPushCmd.Flags().String("remote", "origin", "Remote name")
	viper.BindPFlag("vcs.vcs.push.remote", vcsPushCmd.Flags().Lookup("remote"))
	vcsPushCmd.Flags().String("branch", "", "Branch name (default: current)")
	viper.BindPFlag("vcs.vcs.push.branch", vcsPushCmd.Flags().Lookup("branch"))
	vcsCmd.AddCommand(vcsSetupCmd)
	vcsSetupCmd.Flags().String("token", "", "Personal Access Token")
	viper.BindPFlag("vcs.vcs.setup.token", vcsSetupCmd.Flags().Lookup("token"))
	vcsCmd.AddCommand(vcsStatusCmd)
	vcsCmd.AddCommand(vcsSyncCmd)
}
