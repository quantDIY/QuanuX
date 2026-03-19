package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var installCmd = &cobra.Command{
	Use:   "install [name]",
	Short: "Install a QuanuX extension (Alias for 'ext install')",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("install [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(installCmd)
	installCmd.Flags().StringP("version", "v", "", "")
	viper.BindPFlag("install.install.version", installCmd.Flags().Lookup("version"))
}

