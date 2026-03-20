package output

import (
	"context"
	"encoding/json"
	"fmt"

	"github.com/QuanuX/qxctl/internal/security"
	"github.com/QuanuX/qxctl/internal/theme"
	"gopkg.in/yaml.v3"
)

type key int

const outKey key = 0

// ContextWithManager binds the concrete IO struct natively into context.Context bounds for deep CLI injection.
func ContextWithManager(ctx context.Context, m *Manager) context.Context {
	return context.WithValue(ctx, outKey, m)
}

// FromContext unpacks the singleton natively avoiding Global var leakage.
func FromContext(ctx context.Context) *Manager {
	if m, ok := ctx.Value(outKey).(*Manager); ok {
		return m
	}
	return NewManager("quiet") // Failsafe empty buffer natively.
}

type Manager struct {
	Mode string
}

func NewManager(mode string) *Manager {
	return &Manager{Mode: mode}
}

// EmitRaw natively replaces global fmt.Println usages deeply safely suppressing inside headless bounds natively scrubbing output.
func (m *Manager) EmitRaw(msg ...interface{}) {
	if m.Mode == "quiet" || m.Mode == "json" || m.Mode == "yaml" {
		return
	}
	rawStr := fmt.Sprint(msg...)
	fmt.Println(security.Redact(rawStr))
}

// EmitRawf replaces fmt.Printf gracefully routing targets.
func (m *Manager) EmitRawf(format string, msg ...interface{}) {
	if m.Mode == "quiet" || m.Mode == "json" || m.Mode == "yaml" {
		return
	}
	rawStr := fmt.Sprintf(format, msg...)
	fmt.Print(security.Redact(rawStr))
}

// Print rigidly executes explicit UX formatting constraints per the Config container.
func (m *Manager) Print(data interface{}, humanize func() string) {
	if m.Mode == "quiet" {
		return
	}

	var raw string
	switch m.Mode {
	case "json":
		bytes, _ := json.MarshalIndent(data, "", "  ")
		raw = string(bytes)
	case "yaml":
		bytes, _ := yaml.Marshal(data)
		raw = string(bytes)
	case "text", "table":
		raw = humanize()
	}

	if raw != "" {
		fmt.Println(security.Redact(raw))
	}
}

func (m *Manager) Info(message string) { m.Log("INFO", message) }
func (m *Manager) Warn(message string) { m.Log("WARN", message) }
func (m *Manager) Error(message string) { m.Log("FATAL", message) }
func (m *Manager) Debug(message string) { m.Log("DEBUG", message) }

// Log represents internal diagnostics mapping directly onto theme abstractions.
func (m *Manager) Log(level, message string) {
	if m.Mode == "quiet" || m.Mode == "json" || m.Mode == "yaml" {
		return // Suppress logs in machine paradigms reliably
	}
	
	out := ""
	switch level {
	case "INFO":
		out = theme.DetailStyle.Render("[*] " + message)
	case "WARN":
		out = theme.SkillStyle.Render("[!] " + message)
	case "FATAL":
		out = theme.FailStyle.Render("[X] " + message)
	case "DEBUG":
		out = theme.DetailStyle.Render("[~] " + message)
	}

	fmt.Println(security.Redact(out))
}
