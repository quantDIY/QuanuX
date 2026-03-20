package infra

import (
	"context"
	"fmt"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/zalando/go-keyring"
)

// SetToken abstracts the OS Keyring constraints inside the QuanuX infrastructure package
func SetToken(ctx context.Context, token, target string) error {
	service := "QuanuX"
	user := "do_token"

	err := keyring.Set(service, user, token)
	if err != nil {
		return fmt.Errorf("FATAL: Failed to inject token into the keyring: %w", err)
	}

	output.FromContext(ctx).EmitRaw("SUCCESS: DigitalOcean token securely locked into the OS Keychain.")
	output.FromContext(ctx).EmitRawf("The token will perfectly synchronize with Viper Target: %s\n", target)
	return nil
}
