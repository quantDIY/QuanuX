package cmd

import (
	"github.com/QuanuX/qxctl/pkg/indicators"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var indicatorsCmd = &cobra.Command{
	Use:   "indicators",
	Short: "Manage Indicator Registry (Community)",
}

var indicatorsInstallCmd = &cobra.Command{
	Use:   "install [url]",
	Short: "Install a community indicator from Git",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		name := viper.GetString("indicators.indicators.install.name")
		return indicators.Install(cmd.Context(), args[0], name)
	},
}

var indicatorsListCmd = &cobra.Command{
	Use:   "list",
	Short: "List installed community indicators",
}

var indicatorsRemoveCmd = &cobra.Command{
	Use:   "remove [name]",
	Short: "Remove a community indicator",
}

func init() {
	rootCmd.AddCommand(indicatorsCmd)
	indicatorsCmd.AddCommand(indicatorsInstallCmd)
	indicatorsInstallCmd.Flags().String("name", "", "Local name for the indicator package")
	viper.BindPFlag("indicators.indicators.install.name", indicatorsInstallCmd.Flags().Lookup("name"))
	indicatorsCmd.AddCommand(indicatorsListCmd)
	indicatorsCmd.AddCommand(indicatorsRemoveCmd)
}
