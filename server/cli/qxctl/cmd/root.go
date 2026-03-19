package cmd

import (
	"fmt"
	"os"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var cfgFile string

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use:   "qxctl",
	Short: "QuanuX Control CLI (Go Edition)",
	Long: `qxctl is the ultra-fast, natively compiled Go orchestrator for QuanuX. 
It replaces the Python Typer CLI by providing zero-dependency execution,
native OS Keyring bindings, interactive TUIs via Bubbletea, and real-time
messaging bridging via NATS JetStream and Cython memory matrices.`,
}

// Execute adds all child commands to the root command and sets flags appropriately.
func Execute() error {
	return rootCmd.Execute()
}

func init() {
	cobra.OnInitialize(initConfig)

	// Here you will define your flags and configuration settings.
	// Cobra supports persistent flags, which, if defined here,
	// will be global for your application.

	rootCmd.PersistentFlags().StringVar(&cfgFile, "config", "", "config file (default is $HOME/.qxctl.yaml)")
	rootCmd.PersistentFlags().String("hub", "nats://hub.quanux.io:4222", "NATS Hub URL")

	// Bind flags to Viper
	viper.BindPFlag("hub", rootCmd.PersistentFlags().Lookup("hub"))
}

// initConfig reads in config file and ENV variables if set.
func initConfig() {
	if cfgFile != "" {
		// Use config file from the flag.
		viper.SetConfigFile(cfgFile)
	} else {
		// Find home directory.
		home, err := os.UserHomeDir()
		cobra.CheckErr(err)

		// Search config in home directory with name ".qxctl" (without extension).
		viper.AddConfigPath(home)
		viper.SetConfigType("yaml")
		viper.SetConfigName(".qxctl")
	}

	viper.SetEnvPrefix("QXCTL") // Automatically prefix matched EnvVars like QXCTL_HUB
	viper.AutomaticEnv() // read in environment variables that match

	// If a config file is found, read it in.
	if err := viper.ReadInConfig(); err == nil {
		fmt.Fprintln(os.Stderr, "Using config file:", viper.ConfigFileUsed())
	}
}
