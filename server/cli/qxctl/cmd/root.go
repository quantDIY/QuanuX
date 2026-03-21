package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

// NewRootCmd initializes the base CLI orchestrator and natively binds the explicit
// array of execution commands passing the DI context downstream uniformly.
func NewRootCmd(app *runtime.App) *cobra.Command {
	if app == nil {
		panic("FATAL: runtime.App cannot be nil")
	}

	rootCmd := &cobra.Command{
		Use:   "qxctl",
		Short: "QuanuX Control CLI (Go Edition)",
		Long:  "Operator-grade natively compiled orchestrator managing QuanuX cython grids and HW bounds.",
		PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
			if err := app.Authorize(cmd); err != nil {
				app.Out.ErrorExit(err)
			}
			return nil
		},
	}

	rootCmd.PersistentFlags().String("config", "", "Explicit config file trajectory")
	rootCmd.PersistentFlags().String("hub", "nats://127.0.0.1:4222", "QuanuX JetStream Engine Hub target")
	rootCmd.PersistentFlags().String("output", "text", "Set the output UX mode (text, json, yaml, quiet)")
	rootCmd.PersistentFlags().Bool("trace", false, "Enable extreme diagnostic observability bounds")
	rootCmd.PersistentFlags().Bool("verbose", false, "Enable verbose payload outputs")

	rootCmd.AddCommand(
		NewBridgeCmd(app),
		NewConfigCmd(app),
		NewCrucibleCmd(app),
		NewDashboardCmd(app),
		NewDeployCmd(app),
		NewDisableCmd(app),
		NewEnableCmd(app),
		NewEngineCmd(app),
		NewExtCmd(app),
		NewFoundryCmd(app),
		NewHabitatCmd(app),
		NewIndicatorsCmd(app),
		NewInfraCmd(app),
		NewInstallCmd(app),
		NewLifecycleCmd(app),
		NewModuleCmd(app),
		NewNestCmd(app),
		NewNodeCmd(app),
		NewOrchestraCmd(app),
		NewPredeployCmd(app),
		NewProbeCmd(app),
		NewQueryCmd(app),
		NewRemoveCmd(app),
		NewSecretsCmd(app),
		NewSkillsCmd(app),
		NewSpreaderCmd(app),
		NewStorageCmd(app),
		NewTelemetryCmd(app),
		NewTopstepxCmd(app),
		NewUpgradeCmd(app),
		NewUpgradeableCmd(app),
		NewVaultCmd(app),
		NewVcsCmd(app),
		NewVersionCmd(app),
	)

	return rootCmd
}
