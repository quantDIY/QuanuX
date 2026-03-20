package cmd

import (
	"github.com/QuanuX/qxctl/pkg/habitat"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var habitatCmd = &cobra.Command{
	Use:   "habitat",
	Short: "Manage QuanuX OS-level Habitat Provisioning",
}

var habitatEquipCmd = &cobra.Command{
	Use:   "equip [target]",
	Short: "Equips a raw bare-metal server with the QuanuX OS-level network boundaries  and native C++ toolchains",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		return habitat.Equip(cmd.Context(), args[0])
	},
}

var habitatObserveCmd = &cobra.Command{
	Use:   "observe [target]",
	Short: "Deploys the Panopticon Observability Stack (Ledger, Buffer, and Shadow Node)",
}

var habitatSentinelCmd = &cobra.Command{
	Use:   "sentinel [target]",
	Short: "Deploys the strict Low-Overhead Sentinel Protocol (Telegraf & Promtail)",
}

func init() {
	rootCmd.AddCommand(habitatCmd)
	habitatCmd.AddCommand(habitatEquipCmd)
	habitatCmd.AddCommand(habitatObserveCmd)
	habitatCmd.AddCommand(habitatSentinelCmd)
	habitatSentinelCmd.Flags().String("telemetry-interval", "15s", "Telemetry reporting interval for Telegraf")
	viper.BindPFlag("habitat.habitat.sentinel.telemetry_interval", habitatSentinelCmd.Flags().Lookup("telemetry-interval"))
}
