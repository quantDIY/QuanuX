package cmd

import (
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/vcs"
	"github.com/spf13/cobra"
)

func NewVcsCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "vcs",
		Short: "Version Control System Connectors natively bound without memory leaks",
	}

	cloneCmd := &cobra.Command{
		Use: "clone [url]", Short: "Clone a remote repository", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	cloneCmd.Flags().String("target", "", "Target directory name")

	commitCmd := &cobra.Command{
		Use:   "commit",
		Short: "Commit changes to the local repository",
		RunE: func(cmd *cobra.Command, args []string) error {
			msg, _ := cmd.Flags().GetString("message")
			all, _ := cmd.Flags().GetBool("all")
			if err := vcs.Commit(app.Ctx, msg, all); err != nil {
				return err
			}
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.CommandPath(), Message: "Local VCS state formally committed natively."})
			}
			return nil
		},
	}
	commitCmd.Flags().StringP("message", "m", "", "Commit message")
	commitCmd.Flags().BoolP("all", "a", false, "Stage all modified files")

	runtime.BindMetadata(commitCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	connectCmd := &cobra.Command{
		Use: "connect [url]", Short: "Connect current directory to remote VCS", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	connectCmd.Flags().String("name", "origin", "Remote name")

	publishCmd := &cobra.Command{
		Use: "publish [provider]", Short: "Create remote pushing current project", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	publishCmd.Flags().String("name", "", "Repository name")
	publishCmd.Flags().Bool("private", true, "Create as private")

	pushCmd := &cobra.Command{
		Use: "push", Short: "Push changes to remote", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	pushCmd.Flags().String("remote", "origin", "Remote name")
	pushCmd.Flags().String("branch", "", "Branch name")

	setupCmd := &cobra.Command{
		Use: "setup [provider]", Short: "Configure credentials", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	setupCmd.Flags().String("token", "", "Access Token")

	statusCmd := &cobra.Command{
		Use:   "status",
		Short: "Show status of the current repository natively",
		RunE: func(cmd *cobra.Command, args []string) error {
			return vcs.Status(app.Ctx)
		},
	}

	syncCmd := &cobra.Command{Use: "sync", Short: "Pull latest", RunE: func(cmd *cobra.Command, args []string) error { return nil }}

	cmd.AddCommand(cloneCmd, commitCmd, connectCmd, publishCmd, pushCmd, setupCmd, statusCmd, syncCmd)
	return cmd
}
