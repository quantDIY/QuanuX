package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var storageCmd = &cobra.Command{
	Use:   "storage",
	Short: "Manage connected storage and databases",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("storage invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var storageDuckdbCmd = &cobra.Command{
	Use:   "duckdb",
	Short: "Manage DuckDB cartridges",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("duckdb invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var storageDuckdbAttachCmd = &cobra.Command{
	Use:   "attach [path] [alias]",
	Short: "Attach a DB file to the running QuanuX Server",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("attach [path] [alias] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var storageDuckdbDetachCmd = &cobra.Command{
	Use:   "detach [alias]",
	Short: "Detach a DB file from the QuanuX Server",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("detach [alias] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var storageDuckdbInitCmd = &cobra.Command{
	Use:   "init [path]",
	Short: "Initialize a new empty DuckDB file",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("init [path] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var storageScanCmd = &cobra.Command{
	Use:   "scan",
	Short: "Scan for connected storage arrays and volumes",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("scan invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
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

