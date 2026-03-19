package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var habitatCmd = &cobra.Command{
	Use:   "habitat",
	Short: "Manage QuanuX OS-level Habitat Provisioning",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("habitat invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var habitatEquipCmd = &cobra.Command{
	Use:   "equip [target]",
	Short: "Equips a raw bare-metal server with the QuanuX OS-level network boundaries  and native C++ toolchains",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("equip [target] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var habitatObserveCmd = &cobra.Command{
	Use:   "observe [target]",
	Short: "Deploys the Panopticon Observability Stack (Ledger, Buffer, and Shadow Node)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("observe [target] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var habitatSentinelCmd = &cobra.Command{
	Use:   "sentinel [target]",
	Short: "Deploys the strict Low-Overhead Sentinel Protocol (Telegraf & Promtail)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("sentinel [target] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(habitatCmd)
	habitatCmd.AddCommand(habitatEquipCmd)
	habitatCmd.AddCommand(habitatObserveCmd)
	habitatCmd.AddCommand(habitatSentinelCmd)
	habitatSentinelCmd.Flags().String("telemetry-interval", "15s", "Telemetry reporting interval for Telegraf")
	viper.BindPFlag("habitat.habitat.sentinel.telemetry_interval", habitatSentinelCmd.Flags().Lookup("telemetry-interval"))
}

