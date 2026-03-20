package cmd

import (
	"github.com/QuanuX/qxctl/pkg/storage"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var storageCmd = &cobra.Command{
	Use:   "storage",
	Short: "Manage connected storage and databases",
}

var storageDuckdbCmd = &cobra.Command{
	Use:   "duckdb",
	Short: "Manage DuckDB cartridges",
}

var storageDuckdbAttachCmd = &cobra.Command{
	Use:   "attach [path] [alias]",
	Short: "Attach a DB file to the running QuanuX Server",
}

var storageDuckdbDetachCmd = &cobra.Command{
	Use:   "detach [alias]",
	Short: "Detach a DB file from the QuanuX Server",
}

var storageDuckdbInitCmd = &cobra.Command{
	Use:   "init [path]",
	Short: "Initialize a new empty DuckDB file",
}

var storageScanCmd = &cobra.Command{
	Use:   "scan",
	Short: "Scan for connected storage arrays and volumes",
	RunE: func(cmd *cobra.Command, args []string) error {
		return storage.Scan(cmd.Context())
	},
}

func init() {
	rootCmd.AddCommand(storageCmd)
	storageCmd.AddCommand(storageDuckdbCmd)
	storageDuckdbCmd.AddCommand(storageDuckdbAttachCmd)
	storageDuckdbAttachCmd.Flags().String("server-url", "http://localhost:8000", "QuanuX Server URL")
	viper.BindPFlag("storage.storage.duckdb.attach.server_url", storageDuckdbAttachCmd.Flags().Lookup("server-url"))
	storageDuckdbCmd.AddCommand(storageDuckdbDetachCmd)
	storageDuckdbDetachCmd.Flags().String("server-url", "http://localhost:8000", "QuanuX Server URL")
	viper.BindPFlag("storage.storage.duckdb.detach.server_url", storageDuckdbDetachCmd.Flags().Lookup("server-url"))
	storageDuckdbCmd.AddCommand(storageDuckdbInitCmd)
	storageCmd.AddCommand(storageScanCmd)
}
