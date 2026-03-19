package cmd

import (
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"github.com/QuanuX/qxctl/pkg/skills"
)

var skillsCmd = &cobra.Command{
	Use:   "skills",
	Short: "QuanuX Agent Skills Registry",
	Run: func(cmd *cobra.Command, args []string) {
		cmd.Help()
	},
}

var skillsListCmd = &cobra.Command{
	Use:   "list",
	Short: "List all available agent skills",
	RunE: func(cmd *cobra.Command, args []string) error {
		baseDir := viper.GetString("skills.skills.list.dir")
		return skills.ListSkills(baseDir)
	},
}

var skillsReadCmd = &cobra.Command{
	Use:   "read [skill_name]",
	Short: "Read the SKILL.md for a specific skill",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		skillName := args[0]
		baseDir := viper.GetString("skills.skills.read.dir")
		return skills.ReadSkill(baseDir, skillName)
	},
}

func init() {
	rootCmd.AddCommand(skillsCmd)
	skillsCmd.AddCommand(skillsListCmd)
	skillsCmd.AddCommand(skillsReadCmd)

	skillsListCmd.Flags().String("dir", ".", "Base directory to crawl")
	viper.BindPFlag("skills.skills.list.dir", skillsListCmd.Flags().Lookup("dir"))

	skillsReadCmd.Flags().String("dir", ".", "Base directory to crawl")
	viper.BindPFlag("skills.skills.read.dir", skillsReadCmd.Flags().Lookup("dir"))
}
