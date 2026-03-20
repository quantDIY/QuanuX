package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/habitat"
	"github.com/spf13/cobra"
)

func NewHabitatCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "habitat",
		Short: "Manage QuanuX OS-level Habitat Provisioning",
	}

	equipCmd := &cobra.Command{
		Use:   "equip [target]",
		Short: "Equips a raw bare-metal server with the QuanuX OS-level network boundaries and native C++ toolchains",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			// Using App context cleanly replacing previous context.Background masks
			return habitat.Equip(app.Ctx, args[0])
		},
	}

	observeCmd := &cobra.Command{
		Use:   "observe [target]",
		Short: "Deploys the Panopticon Observability Stack (Ledger, Buffer, and Shadow Node)",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	sentinelCmd := &cobra.Command{
		Use:   "sentinel [target]",
		Short: "Deploys the strict Low-Overhead Sentinel Protocol (Telegraf & Promtail)",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			// interval, _ := cmd.Flags().GetString("telemetry-interval")
			return nil
		},
	}
	sentinelCmd.Flags().String("telemetry-interval", "15s", "Telemetry reporting interval for Telegraf")

	cmd.AddCommand(equipCmd, observeCmd, sentinelCmd)
	return cmd
}
