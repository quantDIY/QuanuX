package output

import (
	"encoding/json"
	"fmt"

	"github.com/QuanuX/qxctl/internal/theme"
	"gopkg.in/yaml.v3"
)

type Manager struct {
	Mode string
}

func NewManager(mode string) *Manager {
	return &Manager{Mode: mode}
}

// Print rigidly executes explicit UX formatting constraints per the Config container.
func (m *Manager) Print(data interface{}, humanize func() string) {
	if m.Mode == "quiet" {
		return
	}

	switch m.Mode {
	case "json":
		bytes, _ := json.MarshalIndent(data, "", "  ")
		fmt.Println(string(bytes))
	case "yaml":
		bytes, _ := yaml.Marshal(data)
		fmt.Print(string(bytes))
	case "text", "table":
		// Native Terminal UI bounds using internal/theme
		out := humanize()
		if out != "" {
			fmt.Println(out)
		}
	}
}

// Log represents internal diagnostics.
func (m *Manager) Log(level, message string) {
	if m.Mode == "quiet" || m.Mode == "json" || m.Mode == "yaml" {
		return // Suppress logs in machine paradigms
	}
	
	switch level {
	case "INFO":
		fmt.Println(theme.DetailStyle.Render("[*] " + message))
	case "WARN":
		fmt.Println(theme.SkillStyle.Render("[!] " + message))
	case "FATAL":
		fmt.Println(theme.FailStyle.Render("[X] " + message))
	}
}
