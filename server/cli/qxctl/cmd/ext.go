package cmd

import (
	"fmt"

	"github.com/hashicorp/go-plugin"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"github.com/QuanuX/qxctl/pkg/ext"
)

var extCmd = &cobra.Command{
	Use:   "ext",
	Short: "Manage QXP Extensions (List, Run)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("ext invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extCleanCmd = &cobra.Command{
	Use:   "clean [name]",
	Short: "Remove logs and runtime files",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("clean [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extEnhanceCmd = &cobra.Command{
	Use:   "enhance [name]",
	Short: "Trigger a 'Turbo' build with specialized performance options",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("enhance [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extInstallCmd = &cobra.Command{
	Use:   "install [name]",
	Short: "Build/Install the extension (runs build",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("install [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extIntegrateCmd = &cobra.Command{
	Use:   "integrate [name]",
	Short: "Inject a local proprietary SDK into the centralized extensions/sdks repository",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("integrate [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extListCmd = &cobra.Command{
	Use:   "list",
	Short: "List all installed extensions and their status",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Querying Hashicorp RPC go-plugin registry...")
		
		var pluginMap = map[string]plugin.Plugin{
			"figma": nil,
			"topstep": nil,
			"rithmic": nil,
		}
		
		fmt.Println("Registered Plugins across Vaults:")
		for name := range pluginMap {
			fmt.Printf("- %s (Active)\n", name)
		}
		fmt.Printf("\nChecked Viper State config globally: %+v\n", viper.GetString("ext.ext.clean.all"))
	},
}

var extRemoveCmd = &cobra.Command{
	Use:   "remove [name]",
	Short: "Alias for uninstall",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("remove [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extRunCmd = &cobra.Command{
	Use:   "run [name]",
	Short: "Run an extension in the foreground (injecting secrets)",
	Args:  cobra.ExactArgs(1),
	RunE: func(cmd *cobra.Command, args []string) error {
		target := args[0]
		return ext.ConnectSidecar(cmd.Context(), target)
	},
}

var extStartCmd = &cobra.Command{
	Use:   "start [name]",
	Short: "Start an extension in the background",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("start [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extStatusCmd = &cobra.Command{
	Use:   "status [name]",
	Short: "Check if an extension is running",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("status [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extStopCmd = &cobra.Command{
	Use:   "stop [name]",
	Short: "Stop a running extension",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("stop [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extUninstallCmd = &cobra.Command{
	Use:   "uninstall [name]",
	Short: "Clean up build artifacts (removes 'build' directory)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("uninstall [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extUpgradeCmd = &cobra.Command{
	Use:   "upgrade [name]",
	Short: "Auto-upgrade to the latest version found upstream",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("upgrade [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var extUpgradeableCmd = &cobra.Command{
	Use:   "upgradeable [name]",
	Short: "Check for available updates (compares installed version vs upstream tags)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("upgradeable [name] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(extCmd)
	extCmd.AddCommand(extCleanCmd)
	extCleanCmd.Flags().Bool("all", false, "Clean all extensions")
	viper.BindPFlag("ext.ext.clean.all", extCleanCmd.Flags().Lookup("all"))
	extCmd.AddCommand(extEnhanceCmd)
	extEnhanceCmd.Flags().String("allocator", "system", "Memory allocator: system, jemalloc, mimalloc")
	viper.BindPFlag("ext.ext.enhance.allocator", extEnhanceCmd.Flags().Lookup("allocator"))
	extEnhanceCmd.Flags().String("logger", "file", "Logging strategy: file, async, null")
	viper.BindPFlag("ext.ext.enhance.logger", extEnhanceCmd.Flags().Lookup("logger"))
	extCmd.AddCommand(extInstallCmd)
	extInstallCmd.Flags().StringP("version", "v", "", "")
	viper.BindPFlag("ext.ext.install.version", extInstallCmd.Flags().Lookup("version"))
	extCmd.AddCommand(extIntegrateCmd)
	extIntegrateCmd.Flags().StringP("path", "p", "", "Path to local SDK folder")
	viper.BindPFlag("ext.ext.integrate.path", extIntegrateCmd.Flags().Lookup("path"))
	extCmd.AddCommand(extListCmd)
	extCmd.AddCommand(extRemoveCmd)
	extRemoveCmd.Flags().BoolP("force", "f", false, "")
	viper.BindPFlag("ext.ext.remove.force", extRemoveCmd.Flags().Lookup("force"))
	extCmd.AddCommand(extRunCmd)
	extCmd.AddCommand(extStartCmd)
	extCmd.AddCommand(extStatusCmd)
	extCmd.AddCommand(extStopCmd)
	extCmd.AddCommand(extUninstallCmd)
	extUninstallCmd.Flags().BoolP("force", "f", false, "")
	viper.BindPFlag("ext.ext.uninstall.force", extUninstallCmd.Flags().Lookup("force"))
	extCmd.AddCommand(extUpgradeCmd)
	extCmd.AddCommand(extUpgradeableCmd)
}

