package runtime

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"os"
	"strings"

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
		// High-risk capabilities subsume their explicit downgrades
		if c == CapDeploy && cap == CapSimulate {
			return true
		}
	}
	return false
}

// parseVaultOIDC simulates provider-interface verification against mocked Vault network responses natively.
func parseVaultOIDC(token string) ([]CapabilityClass, error) {
	if token == "" {
		return nil, fmt.Errorf("token absent (simulating Vault network drop)")
	}
	parts := strings.Split(token, ".")
	if len(parts) != 3 {
		return nil, fmt.Errorf("invalid OIDC JWT format")
	}
	payload, err := base64.RawURLEncoding.DecodeString(parts[1])
	if err != nil {
		return nil, fmt.Errorf("OIDC payload decode failed: %v", err)
	}
	var claims struct {
		Capabilities []CapabilityClass `json:"capabilities"`
	}
	if err := json.Unmarshal(payload, &claims); err != nil {
		return nil, fmt.Errorf("OIDC claims parse failed: %v", err)
	}
	return claims.Capabilities, nil
}

// Authorize systematically intersects evaluation commands against Tranche 3B OIDC bounded tokens.
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

	// Tranche 3B Vault OIDC Pilot
	var tokenStr string
	if flag := cmd.Flag("token"); flag != nil {
		tokenStr = flag.Value.String()
	}
	if tokenStr == "" {
		tokenStr = os.Getenv("QX_VAULT_TOKEN")
	}

	caps, err := parseVaultOIDC(tokenStr)
	if err != nil {
		// Simulating secure defaulting to CAPABILITY_DENIED upon network isolation / missing token.
		return errors.New(errors.CategoryCapabilityDenied, "Authorization rejected. Vault OIDC verification failed or token missing.", err)
	}

	var callerToken = Token{
		Capabilities: caps,
	}

	if !callerToken.HasCapability(requiredClass) {
		message := fmt.Sprintf("Authorization rejected. Caller lacks [%s] capability required to evaluate structural bounds.", requiredClass)
		return errors.New(errors.CategoryCapabilityDenied, message, nil)
	}

	return nil
}
