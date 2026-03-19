package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var indicatorsCmd = &cobra.Command{
	Use:   "indicators",
	Short: "Manage Indicator Registry (Community)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("indicators invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var indicatorsInstallCmd = &cobra.Command{
	Use:   "install [url]",
	Short: "Install a community indicator from Git",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("install [url] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var indicatorsListCmd = &cobra.Command{
	Use:   "list",
	Short: "List installed community indicators",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("list invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var indicatorsRemoveCmd = &cobra.Command{
	Use:   "remove [name]",
	Short: "Remove a community indicator",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("remove [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(indicatorsCmd)
	indicatorsCmd.AddCommand(indicatorsInstallCmd)
	indicatorsInstallCmd.Flags().String("name", "", "Local name for the indicator package")
	viper.BindPFlag("indicators.indicators.install.name", indicatorsInstallCmd.Flags().Lookup("name"))
	indicatorsCmd.AddCommand(indicatorsListCmd)
	indicatorsCmd.AddCommand(indicatorsRemoveCmd)
}

