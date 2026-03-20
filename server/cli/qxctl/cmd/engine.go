package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/engine"
	"github.com/spf13/cobra"
)

func NewEngineCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "engine",
		Short: "Manage QuanuX Edge Engine Tuning & Execution",
	}

	setupTopologyCmd := &cobra.Command{
		Use:   "setup-topology",
		Short: "Configures the core JetStream QUANUX_INGEST stream and dual-consumer routing topology",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	setupTopologyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	startCmd := &cobra.Command{
		Use:   "start",
		Short: "Initializes CNATS, Annex, and Omega Edge pipelines",
		RunE: func(cmd *cobra.Command, args []string) error {
			hubUrl := app.Cfg.HubURL
			target, _ := cmd.Flags().GetString("target")
			return engine.Start(app.Ctx, hubUrl, target)
		},
	}
	startCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	tuneCmd := &cobra.Command{
		Use:   "tune",
		Short: "Invokes kernel and OS tuning playbooks for the designated architecture",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	tuneCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	cmd.AddCommand(setupTopologyCmd, startCmd, tuneCmd)
	return cmd
}
