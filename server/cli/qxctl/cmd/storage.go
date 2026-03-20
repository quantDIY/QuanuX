package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/storage"
	"github.com/spf13/cobra"
)

func NewStorageCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "storage",
		Short: "Manage connected storage and databases natively",
	}

	duckdbCmd := &cobra.Command{
		Use:   "duckdb",
		Short: "Manage DuckDB cartridges statically",
	}

	attachCmd := &cobra.Command{
		Use:   "attach [path] [alias]", Short: "Attach a DB file to the running QuanuX Server", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	attachCmd.Flags().String("server-url", "http://localhost:8000", "QuanuX Server URL")

	detachCmd := &cobra.Command{
		Use:   "detach [alias]", Short: "Detach it securely", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	detachCmd.Flags().String("server-url", "http://localhost:8000", "QuanuX Server URL")

	initCmd := &cobra.Command{
		Use:   "init [path]", Short: "Initialize a new empty DuckDB file natively", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	duckdbCmd.AddCommand(attachCmd, detachCmd, initCmd)

	scanCmd := &cobra.Command{
		Use:   "scan",
		Short: "Scan for connected storage arrays and volumes securely bound via contexts",
		RunE: func(cmd *cobra.Command, args []string) error {
			return storage.Scan(app.Ctx)
		},
	}

	cmd.AddCommand(duckdbCmd, scanCmd)
	return cmd
}
