package cmd_test

import (
	"bytes"
	"os"
	"os/exec"
	"path/filepath"
	"testing"
)

func TestArchitecturalRelapseGuards(t *testing.T) {
	// Task 2 & 5: Negative Tests for Architectural Relapse
	// This physical Sentinel aggressively shields the CI pipeline from lazy engineering.

	wd, _ := os.Getwd()
	cmdDir := filepath.Join(wd)
	rootDir := filepath.Join(wd, "..") // Scan the entirety of qxctl/ natively for Doctrine
	
	tests := []struct {
		name     string
		grepArgs []string
		failMsg  string
	}{
		{
			name:     "Guard against Cloud/Container Doctrine Hallucination",
			grepArgs: []string{"-rniE", "--exclude-dir=.git", "--exclude=qxctl", "--exclude=*_test.go", "--exclude=COMMAND_CENSUS.md", "docker|kubernetes|k8s|cloud-native", rootDir},
			failMsg:  "FATAL RELAPSE: Hallucinated container/orchestrator doctrine detected (Docker/Kubernetes). QuanuX CLI is strictly Bare Metal.",
		},
		{
			name:     "Guard against Global Variables",
			grepArgs: []string{"-rn", "--exclude=*_test.go", "var App", cmdDir},
			failMsg:  "FATAL RELAPSE: 'var App' global state detected! Dependency Injection violated.",
		},
		{
			name:     "Guard against init() registrations",
			grepArgs: []string{"-rn", "--exclude=*_test.go", "func init()", cmdDir},
			failMsg:  "FATAL RELAPSE: 'func init()' detected! Cobra commands MUST be instantiated via NewXCmd.",
		},
		{
			name:     "Guard against Raw OS Output Leaks",
			grepArgs: []string{"-rnE", "--exclude=*_test.go", "fmt\\.Print|os\\.Stdout", cmdDir},
			failMsg:  "FATAL RELAPSE: Raw fmt.Print or os.Stdout detected! All outputs MUST route securely through app.Out for Redaction.",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cmd := exec.Command("grep", tt.grepArgs...)
			var out bytes.Buffer
			cmd.Stdout = &out
			err := cmd.Run()

			// grep returns exit code 1 if NO lines match (which is what we WANT).
			// If it returns 0 (err == nil), it means it FOUND a match. That is a RELAPSE.
			if err == nil {
				t.Fatalf("%s\nViolations Found:\n%s", tt.failMsg, out.String())
			}
		})
	}
}
