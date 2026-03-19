package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var skillsCmd = &cobra.Command{
	Use:   "skills",
	Short: "Manage and discover Agent Skills",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("skills invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var skillsListCmd = &cobra.Command{
	Use:   "list",
	Short: "List all available agent skills",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("list invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var skillsReadCmd = &cobra.Command{
	Use:   "read [skill_name]",
	Short: "Read the content of a specific skill",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("read [skill_name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(skillsCmd)
	skillsCmd.AddCommand(skillsListCmd)
	skillsCmd.AddCommand(skillsReadCmd)
}

