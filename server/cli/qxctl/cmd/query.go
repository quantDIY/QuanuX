package cmd

import (
	"github.com/QuanuX/qxctl/pkg/query"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var queryCmd = &cobra.Command{
	Use:   "query",
	Short: "Manage Analytical Extensions (Validate & Estimate SQL)",
}

var queryEstimateCmd = &cobra.Command{
	Use:   "estimate [sql_file]",
	Short: "Transpiles AST and hits BigQuery Dry-Run API for metrics",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		target := viper.GetString("query.query.estimate.target")
		return query.Estimate(cmd.Context(), args[0], target)
	},
}

var queryValidateCmd = &cobra.Command{
	Use:   "validate [sql_file]",
	Short: "Parses local DuckDB SQL against the Allowed/Banned matrix",
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
