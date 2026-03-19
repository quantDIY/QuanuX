package cmd

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"gopkg.in/yaml.v3"
)

var configCmd = &cobra.Command{
	Use:   "config",
	Short: "Manage the global QuanuX configuration (Viper)",
	Long:  "Initialize or view your ~/.qxctl.yaml file managed entirely by Viper.",
}

var viewCmd = &cobra.Command{
	Use:   "view",
	Short: "View the active Viper configuration state",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("--- Active Viper Configuration ---")
		fmt.Printf("Config File Used: %s\n", viper.ConfigFileUsed())
		
		settings := viper.AllSettings()
		yamlData, err := yaml.Marshal(&settings)
		if err != nil {
			fmt.Printf("Error marshalling config: %v\n", err)
			return
		}
		
		fmt.Println(string(yamlData))
	},
}

var initCmd = &cobra.Command{
	Use:   "init",
	Short: "Initialize a default ~/.qxctl.yaml config file",
	Run: func(cmd *cobra.Command, args []string) {
		home, err := os.UserHomeDir()
		if err != nil {
			fmt.Println("Error locating home directory:", err)
			return
		}

		configPath := filepath.Join(home, ".qxctl.yaml")

		if _, err := os.Stat(configPath); err == nil {
			fmt.Printf("Configuration file already exists at %s\n", configPath)
			return
		}

		// Set baseline QuanuX defaults in Viper
		viper.Set("hub", "nats://hub.quanux.io:4222")
		viper.Set("infra.default_provider", "gcp")
		viper.Set("auth.method", "standard") // Try changing to 'biometric' !
		viper.Set("telemetry.interval", "10s")
		
		err = viper.WriteConfigAs(configPath)
		if err != nil {
			fmt.Printf("Failed to write config file: %v\n", err)
		} else {
			fmt.Printf("Successfully created default configuration at: %s\n", configPath)
		}
	},
}

func init() {
	rootCmd.AddCommand(configCmd)
	configCmd.AddCommand(viewCmd)
	configCmd.AddCommand(initCmd)
}
