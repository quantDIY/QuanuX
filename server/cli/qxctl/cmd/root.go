package cmd

import (
	"context"

	qxerrors "github.com/QuanuX/qxctl/internal/errors"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

// App is the explicit dependency injection container instantiated strictly per-invocation.
var App *runtime.App

var rootCmd = &cobra.Command{
	Use:   "qxctl",
	Short: "QuanuX Control CLI (Go Edition)",
	Long:  "Operator-grade natively compiled orchestrator managing QuanuX cython grids and HW bounds.",
	PersistentPreRunE: func(cmd *cobra.Command, args []string) error {
		cfgFile, _ := cmd.Flags().GetString("config")
		hubURL, _ := cmd.Flags().GetString("hub")
		outputType, _ := cmd.Flags().GetString("output")
		trace, _ := cmd.Flags().GetBool("trace")
		verbose, _ := cmd.Flags().GetBool("verbose")

		overrides := runtime.Overrides{
			ConfigFile: cfgFile,
			HubURL:     hubURL,
			Output:     outputType,
			Trace:      trace,
			Verbose:    verbose,
		}

		app, err := runtime.New(cmd.Context(), overrides)
		if err != nil {
			return qxerrors.New(qxerrors.CategoryUsage, "Runtime architecture failed to initialize", err)
		}
		App = app
		return nil
	},
}

// Execute triggers the native Cobra tree and ensures OS context propagations fall cleanly.
func Execute(ctx context.Context) error {
	return rootCmd.ExecuteContext(ctx)
}

func init() {
	// Flags evaluate natively in Cobra, enforcing separation from configuration logic.
	rootCmd.PersistentFlags().String("config", "", "Explicit config file trajectory")
	rootCmd.PersistentFlags().String("hub", "nats://127.0.0.1:4222", "QuanuX JetStream Engine Hub target")
	rootCmd.PersistentFlags().String("output", "text", "Set the output UX mode (text, json, yaml, quiet)")
	rootCmd.PersistentFlags().Bool("trace", false, "Enable extreme diagnostic observability bounds")
	rootCmd.PersistentFlags().Bool("verbose", false, "Enable verbose payload outputs")
}
