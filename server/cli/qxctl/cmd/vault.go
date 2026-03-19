package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"github.com/QuanuX/qxctl/pkg/vault"
)

var vaultCmd = &cobra.Command{
	Use:   "vault",
	Short: "Manage QuanuX Sovereign Vault and Local HW Cache",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("vault invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var vaultStatusCmd = &cobra.Command{
	Use:   "status",
	Short: "Interrogates the live Annex C++ daemon bounding the Sovereign Vault and NVMe chunk cache",
	RunE: func(cmd *cobra.Command, args []string) error {
		target := viper.GetString("vault.vault.status.target")
		return vault.Status(cmd.Context(), target)
	},
}

func init() {
	rootCmd.AddCommand(vaultCmd)
	vaultCmd.AddCommand(vaultStatusCmd)
	vaultStatusCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("vault.vault.status.target", vaultStatusCmd.Flags().Lookup("target"))
}

