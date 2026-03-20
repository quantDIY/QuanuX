package cmd

import (
	"github.com/QuanuX/qxctl/internal/version"
	"github.com/spf13/cobra"
)

// versionCmd establishes the runtime identification vectors.
var versionCmd = &cobra.Command{
	Use:   "version",
	Short: "Print the deterministic application execution identity",
	Run: func(cmd *cobra.Command, args []string) {
		id := version.Get()

		// Task 8: UX Mode separation. Pass the structured map natively, alongside a humanizer fallback.
		App.Out.Print(id, func() string {
			return id.String() // Fallback to theme-formatted terminal strings for `text` mode.
		})
	},
}

func init() {
	rootCmd.AddCommand(versionCmd)
}
