package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var queryCmd = &cobra.Command{
	Use:   "query",
	Short: "Manage Analytical Extensions (Validate & Estimate SQL)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("query invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var queryEstimateCmd = &cobra.Command{
	Use:   "estimate [sql_file]",
	Short: "Transpiles AST and hits BigQuery Dry-Run API for metrics",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("estimate [sql_file] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var queryValidateCmd = &cobra.Command{
	Use:   "validate [sql_file]",
	Short: "Parses local DuckDB SQL against the Allowed/Banned matrix",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("validate [sql_file] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(queryCmd)
	queryCmd.AddCommand(queryEstimateCmd)
	queryEstimateCmd.Flags().StringP("target", "t", "bq", "Target analytics engine")
	viper.BindPFlag("query.query.estimate.target", queryEstimateCmd.Flags().Lookup("target"))
	queryCmd.AddCommand(queryValidateCmd)
	queryValidateCmd.Flags().StringP("target", "t", "bq", "Target analytics engine")
	viper.BindPFlag("query.query.validate.target", queryValidateCmd.Flags().Lookup("target"))
}

