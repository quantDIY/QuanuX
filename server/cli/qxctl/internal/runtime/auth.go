package runtime

import (
	"encoding/json"
	"fmt"

	"github.com/QuanuX/qxctl/internal/errors"
	"github.com/spf13/cobra"
)

// [PROPOSAL] Token acts as the explicit authorization struct carrying rights.
type Token struct {
	Capabilities []CapabilityClass
}

// HasCapability structurally proves if the array holds the boundary literal.
func (t *Token) HasCapability(cap CapabilityClass) bool {
	for _, c := range t.Capabilities {
		if c == cap {
			return true
		}
	}
	return false
}

// Authorize systematically intersects evaluation commands against [PROPOSAL] tokens.
func (a *App) Authorize(cmd *cobra.Command) error {
	metaRaw, ok := cmd.Annotations["qxctl_metadata"]
	if !ok {
		return nil // Explicitly bypass Uncovered execution endpoints
	}

	var metadata CommandMetadata
	if err := json.Unmarshal([]byte(metaRaw), &metadata); err != nil {
		return errors.New(errors.CategoryInternalBug, "Failed to parse qxctl_metadata dynamically", err)
	}

	requiredClass := metadata.Capability

	// Evaluate downgrade mechanics
	if metadata.SupportsDryRun {
		isDryRun, err := cmd.Flags().GetBool("dry-run")
		if err == nil && isDryRun {
			requiredClass = CapSimulate
		}
	}

	// [PROPOSAL] Synthesize a mock root token for proving bounds locally without remote networking.
	// In production, this would parse from `app.Vault` or JWT Context.
	// We inject `inspect`, `validate`, and `simulate` only. We explicitly do NOT inject `deploy`.
	var callerToken = Token{
		Capabilities: []CapabilityClass{CapInspect, CapValidate, CapSimulate},
	}

	if !callerToken.HasCapability(requiredClass) {
		message := fmt.Sprintf("Authorization rejected. Caller lacks [%s] capability required to evaluate structural bounds.", requiredClass)
		return errors.New(errors.CategoryCapabilityDenied, message, nil)
	}

	return nil
}
