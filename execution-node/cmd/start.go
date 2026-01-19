package cmd

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/QuanuX/QuanuX/execution-node/internal/config"
	"github.com/QuanuX/QuanuX/execution-node/internal/nats"
	natsgo "github.com/nats-io/nats.go"
	"github.com/spf13/cobra"
)

var startCmd = &cobra.Command{
	Use:   "start",
	Short: "Start the Execution Engine daemon",
	Run: func(cmd *cobra.Command, args []string) {
		cfg, err := config.LoadConfig()
		if err != nil {
			fmt.Printf("Error loading config: %v\nRun 'quanux-node register' first.\n", err)
			os.Exit(1)
		}

		fmt.Printf("🚀 Starting Execution Engine (%s)...\n", cfg.NodeID)

		// Initialize NATS Manager
		natsMgr, err := nats.NewManager(cfg)
		if err != nil {
			fmt.Printf("Error initializing NATS manager: %v\n", err)
			os.Exit(1)
		}

		if err := natsMgr.Start(); err != nil {
			fmt.Printf("Error starting NATS leaf node: %v\n", err)
			os.Exit(1)
		}

		// Initialize NATS Client (Connects to the Leaf Node we just started)
		// Give the leaf node a moment to spin up
		time.Sleep(1 * time.Second)

		client := nats.NewClient(cfg)
		if err := client.Connect(); err != nil {
			fmt.Printf("Error connecting to local NATS leaf: %v\n", err)
		} else {
			fmt.Println("✅ Connected to Local NATS Leaf Node")
			defer client.Close()
		}

		// Setup signal handling
		sigChan := make(chan os.Signal, 1)
		signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

		// Main Supervisor Loop
		go func() {
			ticker := time.NewTicker(5 * time.Second)
			defer ticker.Stop()

			for range ticker.C {
				if err := client.PublishHeartbeat(); err != nil {
					// Don't spam errors if NATS is temporarily down (leaf node handling it)
					if err != natsgo.ErrConnectionClosed && err != natsgo.ErrNoServers {
						fmt.Printf("Heartbeat failed: %v\n", err)
					}
				} else {
					fmt.Print(".") // visual feedback
				}
			}
		}()

		<-sigChan
		fmt.Println("\nStopping engine...")
		natsMgr.Stop()
		client.Close()
		fmt.Println("Goodbye!")
	},
}

func init() {
	rootCmd.AddCommand(startCmd)
}
