package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/internal/version"
	"github.com/spf13/cobra"
)

func NewVersionCmd(app *runtime.App) *cobra.Command {
	return &cobra.Command{
		Use:   "version",
		Short: "Print the deterministic application execution identity",
		RunE: func(cmd *cobra.Command, args []string) error {
			id := version.Get()

			// Task 8/C: UX Mode separation accurately passed explicitly inside App closures.
			app.Out.Print(id, func() string {
				return id.String() // Fallback to theme-formatted terminal strings for text mode natively.
			})
			return nil
		},
	}
}
