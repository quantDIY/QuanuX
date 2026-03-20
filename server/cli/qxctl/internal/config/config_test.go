package config

import (
	"os"
	"testing"
)

func TestConfigLoadDefaults(t *testing.T) {
	// Task 12: Lock Test Pyramid
	cfg, err := Load("", "", "", false, false)
	if err != nil {
		t.Fatalf("Expected nil error loading defaults, got: %v", err)
	}

	if cfg.HubURL != "nats://127.0.0.1:4222" {
		t.Errorf("Expected default hub_url nats://127.0.0.1:4222, got %s", cfg.HubURL)
	}
	if cfg.Output != "text" {
		t.Errorf("Expected default output text, got %s", cfg.Output)
	}
}

func TestConfigOverrides(t *testing.T) {
	cfg, err := Load("", "nats://remote.quanux.io:4222", "json", true, true)
	if err != nil {
		t.Fatalf("Expected nil error loading overrides, got: %v", err)
	}

	if cfg.HubURL != "nats://remote.quanux.io:4222" {
		t.Errorf("Expected overridden hub_url, got %s", cfg.HubURL)
	}
	if cfg.Output != "json" {
		t.Errorf("Expected overridden output json, got %s", cfg.Output)
	}
	if !cfg.TraceMode || !cfg.Verbose {
		t.Errorf("Expected true/true for trace/verbose, got %v/%v", cfg.TraceMode, cfg.Verbose)
	}
}

func TestConfigEnvVars(t *testing.T) {
	os.Setenv("QUANUX_HUB_URL", "nats://env.quanux.io:4222")
	defer os.Unsetenv("QUANUX_HUB_URL")

	// Overrides should be empty so it falls back to ENV natively
	cfg, err := Load("", "", "", false, false)
	if err != nil {
		t.Fatalf("Expected nil error loading ENVs, got: %v", err)
	}

	if cfg.HubURL != "nats://env.quanux.io:4222" {
		t.Errorf("Expected ENV-loaded hub_url, got %s", cfg.HubURL)
	}
}

func TestConfigValidationSchema(t *testing.T) {
	_, err := Load("", "", "malformed", false, false)
	if err == nil {
		t.Fatal("Expected validation error on malformed output, got nil")
	}
}
