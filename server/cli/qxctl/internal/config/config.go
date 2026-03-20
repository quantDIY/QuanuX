package config

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/spf13/viper"
)

// Config represents the explicitly typed, schema-validated operator contract.
type Config struct {
	HubURL    string
	Target    string
	Output    string
	TraceMode bool
	Verbose   bool
}

// Load dynamically parses Viper paths natively mapping defaults > files > ENV > CLI overrides.
func Load(configFile, hubURL, output string, trace, verbose bool) (*Config, error) {
	v := viper.New()

	v.SetDefault("hub_url", "nats://127.0.0.1:4222")
	v.SetDefault("target", "gcp")
	v.SetDefault("output", "text")
	v.SetDefault("trace", false)
	v.SetDefault("verbose", false)

	v.SetConfigName("config")
	v.SetConfigType("yaml")

	home, err := os.UserHomeDir()
	if err == nil {
		v.AddConfigPath(filepath.Join(home, ".quanux"))
	}
	v.AddConfigPath("/etc/quanux/")
	v.AddConfigPath(".")

	v.SetEnvPrefix("QUANUX")
	v.AutomaticEnv()

	if configFile != "" {
		v.SetConfigFile(configFile)
	}
	
	_ = v.ReadInConfig()

	// 5. Explicit Flags override physical and env layers absolutely.
	if hubURL != "" && hubURL != "nats://127.0.0.1:4222" {
		v.Set("hub_url", hubURL)
	}
	if output != "" && output != "text" {
		v.Set("output", output)
	}
	if trace {
		v.Set("trace", true)
	}
	if verbose {
		v.Set("verbose", true)
	}

	cfg := &Config{
		HubURL:    v.GetString("hub_url"),
		Target:    v.GetString("target"),
		Output:    v.GetString("output"),
		TraceMode: v.GetBool("trace"),
		Verbose:   v.GetBool("verbose"),
	}

	if err := cfg.Validate(); err != nil {
		return nil, fmt.Errorf("configuration schema invalid: %w", err)
	}

	return cfg, nil
}

// Validate rigidly enforces schema limits before any operator tools boot.
func (c *Config) Validate() error {
	if c.HubURL == "" {
		return fmt.Errorf("hub_url cannot be completely naked")
	}
	if c.Output != "text" && c.Output != "json" && c.Output != "yaml" && c.Output != "table" && c.Output != "quiet" {
		return fmt.Errorf("invalid strict output mode specified: %s", c.Output)
	}
	return nil
}
