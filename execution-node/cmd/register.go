package cmd

import (
	"fmt"

	"github.com/QuanuX/QuanuX/execution-node/internal/config"
	"github.com/google/uuid"
	"github.com/spf13/cobra"
)

var (
	hubURL string
	token  string
	name   string
)

var registerCmd = &cobra.Command{
	Use:   "register",
	Short: "Register this node with the QuanuX Hub",
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Printf("Registering node at %s...\n", hubURL)

		// Mock Registration Logic
		// In production, this would exchange 'token' for NATS creds via HTTPS or NATS
		nodeID := "node-" + uuid.New().String()[:8]

		cfg := &config.NodeConfig{
			NodeID:       nodeID,
			FriendlyName: name,
			Hub: &config.HubConfig{
				URL:  hubURL,
				JWT:  "mock-jwt-token",
				Seed: "mock-nkey-seed",
			},
			NatsPort: 4222,
			LeafPort: 7422,
		}

		if cfg.FriendlyName == "" {
			cfg.FriendlyName = nodeID
		}

		if err := config.SaveConfig(cfg); err != nil {
			fmt.Printf("Error saving config: %v\n", err)
			return
		}

		fmt.Printf("✅ Registered successfully! ID: %s\n", nodeID)
		fmt.Println("You can now run 'quanux-node start'")
	},
}

func init() {
	rootCmd.AddCommand(registerCmd)

	registerCmd.Flags().StringVar(&hubURL, "hub", "", "NATS Hub URL (required)")
	registerCmd.MarkFlagRequired("hub")
	registerCmd.Flags().StringVar(&token, "token", "", "Registration Token (required)")
	registerCmd.MarkFlagRequired("token")
	registerCmd.Flags().StringVar(&name, "name", "", "Friendly name for this node")
}
