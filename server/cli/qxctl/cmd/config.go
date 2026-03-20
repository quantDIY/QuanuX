package cmd

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

func NewConfigCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "config",
		Short: "Manage the global QuanuX configuration natively",
	}

	viewCmd := &cobra.Command{
		Use:   "view",
		Short: "View the active configuration struct",
		RunE: func(cmd *cobra.Command, args []string) error {
			// Output firmly through Emit/Print bounds handling JSON cleanly!
			app.Out.Print(app.Cfg, func() string {
				return fmt.Sprintf("--- Active Configuration ---\nHubURL: %s\nTarget: %s\nOutput: %s\nVerbose: %v",
					app.Cfg.HubURL, app.Cfg.Target, app.Cfg.Output, app.Cfg.Verbose)
			})
			return nil
		},
	}

	initCmd := &cobra.Command{
		Use:   "init",
		Short: "Initialize a default ~/.qxctl.yaml config file",
		RunE: func(cmd *cobra.Command, args []string) error {
			home, err := os.UserHomeDir()
			if err != nil {
				return fmt.Errorf("error locating home directory: %v", err)
			}
			configPath := filepath.Join(home, ".qxctl.yaml")
			if _, err := os.Stat(configPath); err == nil {
				app.Out.Log("INFO", fmt.Sprintf("Configuration file already exists at %s", configPath))
				return nil
			}
			
			// This generates natively without Viper directly.
			defaultConfig := []byte("hub_url: nats://hub.quanux.io:4222\ntarget: gcp\noutput: text\n")
			if err := os.WriteFile(configPath, defaultConfig, 0644); err != nil {
				return fmt.Errorf("failed to write config file: %v", err)
			}
			app.Out.Log("INFO", fmt.Sprintf("Successfully created default configuration at: %s", configPath))
			return nil
		},
	}

	cmd.AddCommand(viewCmd, initCmd)
	return cmd
}
