package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/skills"
	"github.com/spf13/cobra"
)

func NewSkillsCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "skills",
		Short: "QuanuX Agent Skills Registry",
		Run: func(cmd *cobra.Command, args []string) {
			cmd.Help()
		},
	}

	listCmd := &cobra.Command{
		Use:   "list",
		Short: "List all available agent skills",
		RunE: func(cmd *cobra.Command, args []string) error {
			baseDir, _ := cmd.Flags().GetString("dir")
			return skills.ListSkills(app.Ctx, baseDir)
		},
	}
	listCmd.Flags().String("dir", ".", "Base directory to crawl")

	readCmd := &cobra.Command{
		Use:   "read [skill_name]",
		Short: "Read the SKILL.md for a specific skill",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			baseDir, _ := cmd.Flags().GetString("dir")
			return skills.ReadSkill(app.Ctx, baseDir, args[0])
		},
	}
	readCmd.Flags().String("dir", ".", "Base directory to crawl")

	cmd.AddCommand(listCmd, readCmd)
	return cmd
}
