package cmd

import (
	"fmt"
	"io/ioutil"
	"path/filepath"
	"strings"
	"os"

	"github.com/charmbracelet/lipgloss"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var (
	skillHeaderStyle = lipgloss.NewStyle().Bold(true).Foreground(lipgloss.Color("#00FF00")).MarginBottom(1)
	skillNameStyle  = lipgloss.NewStyle().Foreground(lipgloss.Color("#00BFFF")).Bold(true)
	skillDescStyle   = lipgloss.NewStyle().Foreground(lipgloss.Color("#AAAAAA")).PaddingLeft(4)
)

var skillsCmd = &cobra.Command{
	Use:   "skills",
	Short: "Manage and discover Agent Skills",
	Run: func(cmd *cobra.Command, args []string) {
		cmd.Help()
	},
}

var skillsListCmd = &cobra.Command{
	Use:   "list",
	Short: "List all available agent skills",
	Run: func(cmd *cobra.Command, args []string) {
		baseDir := viper.GetString("skills.skills.list.dir")

		fmt.Println(skillHeaderStyle.Render(fmt.Sprintf("QuanuX Global Skills Registry (Path: %s):", baseDir)))

		found := 0

		err := filepath.Walk(baseDir, func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return nil
			}
			if info.IsDir() && (info.Name() == ".git" || info.Name() == "venv" || info.Name() == "node_modules" || info.Name() == "__pycache__") {
				return filepath.SkipDir
			}
			
			nameLower := strings.ToLower(info.Name())
			if !info.IsDir() && (nameLower == "skill.md" || nameLower == "skills.md") {
				skillName := filepath.Base(filepath.Dir(path))
				
				fmt.Println(skillNameStyle.Render("► " + strings.ReplaceAll(strings.Title(skillName), "_", " ")))
				
				content, _ := ioutil.ReadFile(path)
				lines := strings.Split(string(content), "\n")
				desc := "No description provided."
				for _, line := range lines {
					if strings.HasPrefix(line, "description: ") {
						desc = strings.TrimPrefix(line, "description: ")
						break
					}
				}
				
				fmt.Println(skillDescStyle.Render(desc) + "\n")
				found++
			}
			return nil
		})

		if err != nil {
			fmt.Printf("Error walking skills directory: %v\n", err)
			return
		}

		if found == 0 {
			fmt.Println("No active skills mapped in the directory.")
		} else {
			fmt.Println(skillHeaderStyle.Render(fmt.Sprintf("%d Active Skills Loaded.", found)))
		}
	},
}

var skillsReadCmd = &cobra.Command{
	Use:   "read [skill_name]",
	Short: "Read the content of a specific skill",
	Args:  cobra.ExactArgs(1),
	Run: func(cmd *cobra.Command, args []string) {
		skillName := args[0]
		baseDir := viper.GetString("skills.skills.read.dir")
        
		var foundPath string
		_ = filepath.Walk(baseDir, func(path string, info os.FileInfo, err error) error {
			if err != nil {
				return nil
			}
			if info.IsDir() && (info.Name() == ".git" || info.Name() == "venv" || info.Name() == "node_modules" || info.Name() == "__pycache__") {
				return filepath.SkipDir
			}
			nameLower := strings.ToLower(info.Name())
			if !info.IsDir() && (nameLower == "skill.md" || nameLower == "skills.md") && filepath.Base(filepath.Dir(path)) == skillName {
				foundPath = path
			}
			return nil
		})
        
        if foundPath == "" {
            fmt.Printf("Error: Could not find skill '%s' in any directory.\n", skillName)
            return
        }
        
        content, _ := ioutil.ReadFile(foundPath)
        
        fmt.Println(skillHeaderStyle.Render(fmt.Sprintf("--- %s ---\n", strings.ToUpper(skillName))))
        fmt.Println(string(content))
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

