package config

import (
	"os"
	"path/filepath"

	"gopkg.in/yaml.v3"
)

type HubConfig struct {
	URL  string `yaml:"url"`
	JWT  string `yaml:"jwt,omitempty"`
	Seed string `yaml:"seed,omitempty"`
}

type NodeConfig struct {
	NodeID       string     `yaml:"node_id"`
	FriendlyName string     `yaml:"friendly_name"`
	Hub          *HubConfig `yaml:"hub,omitempty"`
	NatsPort     int        `yaml:"nats_port"`
	LeafPort     int        `yaml:"leaf_port"`
}

func GetConfigDir() (string, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return "", err
	}
	return filepath.Join(home, ".quanux-node"), nil
}

func LoadConfig() (*NodeConfig, error) {
	dir, err := GetConfigDir()
	if err != nil {
		return nil, err
	}

	path := filepath.Join(dir, "config.yaml")
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	var cfg NodeConfig
	if err := yaml.Unmarshal(data, &cfg); err != nil {
		return nil, err
	}

	return &cfg, nil
}

func SaveConfig(cfg *NodeConfig) error {
	dir, err := GetConfigDir()
	if err != nil {
		return err
	}

	if err := os.MkdirAll(dir, 0755); err != nil {
		return err
	}

	data, err := yaml.Marshal(cfg)
	if err != nil {
		return err
	}

	return os.WriteFile(filepath.Join(dir, "config.yaml"), data, 0644)
}
