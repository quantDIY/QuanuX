package cmd

import (
	"github.com/QuanuX/qxctl/pkg/infra"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var infraCmd = &cobra.Command{
	Use:   "infra",
	Short: "QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning",
}

var infraApplyCmd = &cobra.Command{
	Use:   "apply",
	Short: "Runs Terraform Apply strictly for the designated deployment",
	RunE: func(cmd *cobra.Command, args []string) error {
		target := viper.GetString("infra.infra.apply.target")
		return infra.Apply(cmd.Context(), target)
	},
}

var infraAuthCmd = &cobra.Command{
	Use:   "auth",
	Short: "Outputs export commands for QECD Phase 3 Terraform Provisioning",
}

var infraDestroyCmd = &cobra.Command{
	Use:   "destroy",
	Short: "Destroys the designated QuanuX Deployment",
}

var infraGcpSqlCmd = &cobra.Command{
	Use:   "gcp-sql",
	Short: "GCP Bounded AST SQL Transpilation",
}

var infraGcpSqlExecuteCmd = &cobra.Command{
	Use:   "execute [query]",
	Short: "Validates, transpiles, and executes bounded SQL against BigQuery",
}

var infraGcpSqlTranspileCmd = &cobra.Command{
	Use:   "transpile [query]",
	Short: "Emits translated BigQuery SQL if within the approved Phase 1 matrix",
}

var infraGcpSqlValidateCmd = &cobra.Command{
	Use:   "validate [query]",
	Short: "Validates if the query is within the approved Phase 1 matrix",
}

var infraIngestStartCmd = &cobra.Command{
	Use:   "ingest-start",
	Short: "Starts the QuanuX asynchronous ingestion pipeline",
}

var infraSetSpacesKeysCmd = &cobra.Command{
	Use:   "set-spaces-keys [access_id] [secret_key]",
	Short: "Securely saves the DigitalOcean Spaces keys to the native OS Keyring",
}

var infraSetTokenCmd = &cobra.Command{
	Use:   "set-token [token]",
	Short: "Securely saves the DigitalOcean API Token to the native OS Keyring (Zero-Disk storage)",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		token := args[0]
		target := viper.GetString("infra.infra.status.target")
		return infra.SetToken(token, target)
	},
}

var infraStatusCmd = &cobra.Command{
	Use:   "status",
	Short: "Lists active nodes and vaults from terraform output",
}

var infraTableRegisterCmd = &cobra.Command{
	Use:   "table-register",
	Short: "Registers an external table against the data lake",
}

func init() {
	rootCmd.AddCommand(infraCmd)
	infraCmd.AddCommand(infraApplyCmd)
	infraApplyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("infra.infra.apply.target", infraApplyCmd.Flags().Lookup("target"))
	infraCmd.AddCommand(infraAuthCmd)
	infraAuthCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("infra.infra.auth.target", infraAuthCmd.Flags().Lookup("target"))
	infraCmd.AddCommand(infraDestroyCmd)
	infraDestroyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("infra.infra.destroy.target", infraDestroyCmd.Flags().Lookup("target"))
	infraCmd.AddCommand(infraGcpSqlCmd)
	infraGcpSqlCmd.AddCommand(infraGcpSqlExecuteCmd)
	infraGcpSqlExecuteCmd.Flags().Int("max-rows", 100, "Maximum rows to fetch remotely")
	viper.BindPFlag("infra.infra.gcp_sql.execute.max_rows", infraGcpSqlExecuteCmd.Flags().Lookup("max-rows"))
	infraGcpSqlExecuteCmd.Flags().Bool("dry-run", false, "Validate and transpile only, do not send to GCP")
	viper.BindPFlag("infra.infra.gcp_sql.execute.dry_run", infraGcpSqlExecuteCmd.Flags().Lookup("dry-run"))
	infraGcpSqlExecuteCmd.Flags().Int("timeout", 30, "Timeout in seconds for remote execution")
	viper.BindPFlag("infra.infra.gcp_sql.execute.timeout", infraGcpSqlExecuteCmd.Flags().Lookup("timeout"))
	infraGcpSqlExecuteCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")
	viper.BindPFlag("infra.infra.gcp_sql.execute.json", infraGcpSqlExecuteCmd.Flags().Lookup("json"))
	infraGcpSqlCmd.AddCommand(infraGcpSqlTranspileCmd)
	infraGcpSqlTranspileCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")
	viper.BindPFlag("infra.infra.gcp_sql.transpile.json", infraGcpSqlTranspileCmd.Flags().Lookup("json"))
	infraGcpSqlCmd.AddCommand(infraGcpSqlValidateCmd)
	infraGcpSqlValidateCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")
	viper.BindPFlag("infra.infra.gcp_sql.validate.json", infraGcpSqlValidateCmd.Flags().Lookup("json"))
	infraCmd.AddCommand(infraIngestStartCmd)
	infraIngestStartCmd.Flags().String("target", "gcp", "Cloud target (do or gcp)")
	viper.BindPFlag("infra.infra.ingest_start.target", infraIngestStartCmd.Flags().Lookup("target"))
	infraIngestStartCmd.Flags().Int("memory-limit-mb", 500, "Memory limit in MB for JetStream batching")
	viper.BindPFlag("infra.infra.ingest_start.memory_limit_mb", infraIngestStartCmd.Flags().Lookup("memory-limit-mb"))
	infraCmd.AddCommand(infraSetSpacesKeysCmd)
	infraCmd.AddCommand(infraSetTokenCmd)
	infraCmd.AddCommand(infraStatusCmd)
	infraStatusCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("infra.infra.status.target", infraStatusCmd.Flags().Lookup("target"))
	infraCmd.AddCommand(infraTableRegisterCmd)
	infraTableRegisterCmd.Flags().String("target", "gcp", "Cloud target (do or gcp)")
	viper.BindPFlag("infra.infra.table_register.target", infraTableRegisterCmd.Flags().Lookup("target"))
	infraTableRegisterCmd.Flags().String("project", "", "GCP Project ID")
	viper.BindPFlag("infra.infra.table_register.project", infraTableRegisterCmd.Flags().Lookup("project"))
	infraTableRegisterCmd.Flags().String("uri", "", "GCS URI for Parquet files")
	viper.BindPFlag("infra.infra.table_register.uri", infraTableRegisterCmd.Flags().Lookup("uri"))
}
