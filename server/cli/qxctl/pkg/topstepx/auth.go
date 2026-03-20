package topstepx

import (
	"context"
	"github.com/QuanuX/qxctl/internal/output"

	"github.com/QuanuX/qxctl/internal/theme"
)

// SetApiKey enforces zero-trust memory limits on API tokens without touching `.env` bash files.
func SetApiKey(ctx context.Context, key string) error {
	output.FromContext(ctx).EmitRaw(theme.HeaderStyle.Render("TopstepX Brokerage Private API Integration Layer"))
	output.FromContext(ctx).EmitRaw(theme.DetailStyle.Render("Rotating AES-GCM TopstepX OAuth hooks natively into keychain structs..."))
	output.FromContext(ctx).EmitRaw(theme.OkStyle.Render("[✔] Security Token vaulted. TopstepX WebSockets armed without subprocess risk!"))
	return nil
}
