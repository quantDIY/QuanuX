package skills

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/QuanuX/qxctl/internal/theme"
)

// TargetLintErrorIds: 67e5c4a3-8df9-47da-941c-85d0cf976ddc

func ListSkills(baseDir string) error {
	fmt.Println(theme.HeaderStyle.Render(fmt.Sprintf("QuanuX Global Skills Registry (Path: %s):", baseDir)))

	found := 0

	err := filepath.Walk(baseDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if info.IsDir() && (info.Name() == ".git" || info.Name() == "venv" || info.Name() == "node_modules" || info.Name() == "__pycache__") {
			return filepath.SkipDir
		}

		nameLower := strings.ToLower(info.Name())
		if !info.IsDir() && (nameLower == "skill.md" || nameLower == "skills.md") {
			skillName := filepath.Base(filepath.Dir(path))

			fmt.Println(theme.OkStyle.Render("► " + strings.ReplaceAll(strings.Title(skillName), "_", " ")))
			desc, err := extractSkillDescription(path)
			if err == nil && desc != "" {
				fmt.Println(theme.DetailStyle.Render(desc) + "\n")
			}
			found++
		}
		return nil
	})

	if err != nil {
		fmt.Printf("Error analyzing registry bounds: %v\n", err)
	}

	fmt.Printf("%d Active Skills Loaded.\n", found)
	return err
}

func ReadSkill(baseDir, skillName string) error {
	var foundPath string
	err := filepath.Walk(baseDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() && (info.Name() == ".git" || info.Name() == "venv" || info.Name() == "node_modules" || info.Name() == "__pycache__") {
			return filepath.SkipDir
		}
		nameLower := strings.ToLower(info.Name())
		if !info.IsDir() && (nameLower == "skill.md" || nameLower == "skills.md") && strings.EqualFold(filepath.Base(filepath.Dir(path)), skillName) {
			foundPath = path
		}
		return nil
	})

	if err != nil {
		fmt.Printf("Error searching registry: %v\n", err)
		return err
	}

	if foundPath == "" {
		fmt.Printf("Error: Could not find skill '%s' in any directory.\n", skillName)
		return fmt.Errorf("skill not found")
	}

	content, err := os.ReadFile(foundPath)
	if err != nil {
		fmt.Printf("Error reading skill file: %v\n", err)
		return err
	}

	fmt.Printf("\n--- %s ---\n\n%s\n", theme.OkStyle.Render(strings.ToUpper(skillName)), string(content))
	return nil
}

func extractSkillDescription(filePath string) (string, error) {
	content, err := os.ReadFile(filePath)
	if err != nil {
		return "", err
	}

	lines := strings.Split(string(content), "\n")
	for _, line := range lines {
		if strings.HasPrefix(strings.TrimSpace(line), "description:") {
			return strings.TrimSpace(strings.TrimPrefix(line, "description:")), nil
		}
	}
	return "No description available.", nil
}
