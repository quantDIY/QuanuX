package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/vault"
	"github.com/spf13/cobra"
)

func NewVaultCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "vault",
		Short: "Manage QuanuX Sovereign Vault and Local HW Cache",
	}

	statusCmd := &cobra.Command{
		Use:   "status",
		Short: "Interrogates the live Annex C++ daemon bounding the Sovereign Vault and NVMe chunk cache",
		RunE: func(cmd *cobra.Command, args []string) error {
			target, _ := cmd.Flags().GetString("target")
			return vault.Status(app.Ctx, target)
		},
	}
	statusCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	runtime.BindMetadata(statusCmd, runtime.CommandMetadata{
		Capability:          runtime.CapInspect,
		Risk:                runtime.RiskStable,
		IsIdempotent:        true,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	cmd.AddCommand(statusCmd)
	return cmd
}
