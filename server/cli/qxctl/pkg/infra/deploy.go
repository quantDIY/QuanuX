package infra

import (
	"context"
	"fmt"
	"os"
	"path/filepath"

	"github.com/hashicorp/go-version"
	"github.com/hashicorp/hc-install/product"
	"github.com/hashicorp/hc-install/releases"
	"github.com/hashicorp/terraform-exec/tfexec"
)

// Apply statically allocates Terraform Workspaces natively in memory instead of relying on Bash execution boundaries.
func Apply(ctx context.Context, target string) error {
	fmt.Printf("Initialising Native Terraform Workspace Context Target: [%s]\n", target)

	home, _ := os.UserHomeDir()
	repoRoot := filepath.Join(home, "Antigravity", "QuanuX", "QuanuX")
	workingDir := filepath.Join(repoRoot, "QuanuX-Infra", "terraform", target)

	if _, err := os.Stat(workingDir); os.IsNotExist(err) {
		return fmt.Errorf("FATAL: QuanuX Infrastructure bounds do not exist exactly at mapped vector: %s", workingDir)
	}

	fmt.Println("Locating active TF system binaries natively...")
	installer := &releases.ExactVersion{
		Product: product.Terraform,
		Version: version.Must(version.NewVersion("1.5.7")),
	}

	execPath, err := installer.Install(ctx)
	if err != nil {
		return fmt.Errorf("error resolving system terraform path via hc-install: %w", err)
	}

	fmt.Println("Mapping API to TF Workspace...")
	tf, err := tfexec.NewTerraform(workingDir, execPath)
	if err != nil {
		return fmt.Errorf("error allocating tf workspace in native ram: %w", err)
	}

	// Hook active Terraform print statements securely into the native CLI
	tf.SetStdout(os.Stdout)
	tf.SetStderr(os.Stderr)

	fmt.Println("Initializing module matrix via tf.Init()...")
	err = tf.Init(ctx, tfexec.Upgrade(true))
	if err != nil {
		return fmt.Errorf("error completing module matrix initialisation: %w", err)
	}

	fmt.Println("Execution safety interlock: Running tf.Plan() first explicitly before native writes!")
	// Execute Phase 3 Terraform mappings locally safely
	planHasChanges, err := tf.Plan(ctx)
	if err != nil {
		return fmt.Errorf("error executing native plan state logic: %w", err)
	}

	if planHasChanges {
		fmt.Println("SUCCESS: Complex Native Terraform Matrix calculated. Remote and local contexts drift heavily and require Auto-Suture syncing!")
	} else {
		fmt.Println("SUCCESS: Terraform Context matches local state perfectly organically! Zero drift registered.")
	}

	return nil
}
