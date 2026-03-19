package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var predeployCmd = &cobra.Command{
	Use:   "predeploy",
	Short: "Performs a Capability Handshake prior to deployment",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("predeploy invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(predeployCmd)
	predeployCmd.Flags().StringP("payload", "p", "", "")
	viper.BindPFlag("predeploy.predeploy.payload", predeployCmd.Flags().Lookup("payload"))
	predeployCmd.Flags().StringP("target", "t", "", "")
	viper.BindPFlag("predeploy.predeploy.target", predeployCmd.Flags().Lookup("target"))
	predeployCmd.Flags().String("type", "extension", "")
	viper.BindPFlag("predeploy.predeploy.type", predeployCmd.Flags().Lookup("type"))
}

