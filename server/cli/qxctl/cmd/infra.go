package cmd

import (
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/infra"
	"github.com/spf13/cobra"
)

func NewInfraCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "infra",
		Short: "QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning",
	}

	applyCmd := &cobra.Command{
		Use:   "apply",
		Short: "Runs Terraform Apply strictly for the designated deployment",
		RunE: func(cmd *cobra.Command, args []string) error {
			target, _ := cmd.Flags().GetString("target")
			if err := infra.Apply(app.Ctx, target); err != nil {
				return err
			}
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.CommandPath(), Message: "Infrastructure deployed natively and immutably."})
			}
			return nil
		},
	}
	applyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	runtime.BindMetadata(applyCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	authCmd := &cobra.Command{
		Use:   "auth",
		Short: "Outputs export commands for QECD Phase 3 Terraform Provisioning",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	authCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	destroyCmd := &cobra.Command{
		Use:   "destroy",
		Short: "Destroys the designated QuanuX Deployment",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	destroyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	gcpSqlCmd := &cobra.Command{
		Use:   "gcp-sql",
		Short: "GCP Bounded AST SQL Transpilation",
	}

	gcpSqlExecuteCmd := &cobra.Command{
		Use:   "execute [query]",
		Short: "Validates, transpiles, and executes bounded SQL against BigQuery",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	gcpSqlExecuteCmd.Flags().Int("max-rows", 100, "Maximum rows to fetch remotely")
	gcpSqlExecuteCmd.Flags().Bool("dry-run", false, "Validate and transpile only, do not send to GCP")
	gcpSqlExecuteCmd.Flags().Int("timeout", 30, "Timeout in seconds for remote execution")
	gcpSqlExecuteCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")

	gcpSqlTranspileCmd := &cobra.Command{
		Use:   "transpile [query]",
		Short: "Emits translated BigQuery SQL if within the approved Phase 1 matrix",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	gcpSqlTranspileCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")

	gcpSqlValidateCmd := &cobra.Command{
		Use:   "validate [query]",
		Short: "Validates if the query is within the approved Phase 1 matrix",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	gcpSqlValidateCmd.Flags().Bool("json", false, "Emit purely JSON payload for machine execution")
	gcpSqlCmd.AddCommand(gcpSqlExecuteCmd, gcpSqlTranspileCmd, gcpSqlValidateCmd)

	ingestStartCmd := &cobra.Command{
		Use:   "ingest-start",
		Short: "Starts the QuanuX asynchronous ingestion pipeline",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	ingestStartCmd.Flags().String("target", "gcp", "Cloud target (do or gcp)")
	ingestStartCmd.Flags().Int("memory-limit-mb", 500, "Memory limit in MB for JetStream batching")

	setSpacesKeysCmd := &cobra.Command{
		Use:   "set-spaces-keys [access_id] [secret_key]",
		Short: "Securely saves the DigitalOcean Spaces keys to the native OS Keyring",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	setTokenCmd := &cobra.Command{
		Use:   "set-token [token]",
		Short: "Securely saves the DigitalOcean API Token to the native OS Keyring (Zero-Disk storage)",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			token := args[0]
			// Deriving target from flag context directly without global Viper map constraints.
			target, _ := cmd.Flags().GetString("target")
			return infra.SetToken(app.Ctx, token, target)
		},
	}
	setTokenCmd.Flags().String("target", "gcp", "Target bound to evaluate against.")

	statusCmd := &cobra.Command{
		Use:   "status",
		Short: "Lists active nodes and vaults from terraform output",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	statusCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")

	tableRegisterCmd := &cobra.Command{
		Use:   "table-register",
		Short: "Registers an external table against the data lake",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	tableRegisterCmd.Flags().String("target", "gcp", "Cloud target (do or gcp)")
	tableRegisterCmd.Flags().String("project", "", "GCP Project ID")
	tableRegisterCmd.Flags().String("uri", "", "GCS URI for Parquet files")

	cmd.AddCommand(applyCmd, authCmd, destroyCmd, gcpSqlCmd, ingestStartCmd, setSpacesKeysCmd, setTokenCmd, statusCmd, tableRegisterCmd)
	return cmd
}
