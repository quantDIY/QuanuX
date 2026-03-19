package vault

import (
	"context"
	"fmt"
	"os"

	"github.com/hashicorp/vault/api"
	"github.com/QuanuX/qxctl/internal/theme"
)

// Status probes the global Zero-Trust Hashicorp Vault infrastructure directly in memory instead of executing bash scripts natively.
func Status(ctx context.Context, target string) error {
	fmt.Println(theme.HeaderStyle.Render(fmt.Sprintf("QuanuX Sovereign Vault Native Interlock (Target: %s)", target)))

	// Initialize Native HashiCorp Vault Client
	config := api.DefaultConfig()
	config.Address = os.Getenv("VAULT_ADDR") 

	if config.Address == "" {
		// Default to local dev loop if env bound not mapped
		config.Address = "http://127.0.0.1:8200"
	}

	client, err := api.NewClient(config)
	if err != nil {
		fmt.Println(theme.FailStyle.Render("[X] Failed to allocate HashiCorp Vault API structs natively."))
		return err
	}

	fmt.Println(theme.DetailStyle.Render(fmt.Sprintf("Pinging QuanuX PKI Engine natively at %s...", config.Address)))
	
	health, err := client.Sys().Health()
	if err != nil {
		fmt.Println(theme.FailStyle.Render("[X] Sovereign Vault is OFFLINE or physically inaccessible."))
		return nil
	}

	if health.Initialized && !health.Sealed {
		fmt.Println(theme.OkStyle.Render("[✔] Vault is UNSEALED and supplying Active OIDC hardware dynamically."))
		fmt.Println(theme.SkillStyle.Render(fmt.Sprintf("Engine Version: %s | Cluster Identity: %s", health.Version, health.ClusterName)))
	} else if health.Sealed {
		fmt.Println(theme.FailStyle.Render("[!] Vault is SEALED. Awaiting hardware Shamir unseal keys."))
	}

	return nil
}
