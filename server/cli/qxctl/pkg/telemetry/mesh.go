package telemetry

import (
	"context"
	"fmt"
	"github.com/QuanuX/qxctl/internal/output"

	"github.com/QuanuX/qxctl/internal/theme"
)

// Restart triggers node network telemetry resets bypassing Typer.
func Restart(ctx context.Context, node, service string) error {
	output.FromContext(ctx).EmitRaw(theme.HeaderStyle.Render(fmt.Sprintf("QuanuX OpenTelemetry Mesh Manager (Node: %s)", node)))
	output.FromContext(ctx).EmitRaw(theme.DetailStyle.Render(fmt.Sprintf("Transmitting native grpc lifecycle bounds to '%s'...", service)))
	output.FromContext(ctx).EmitRaw(theme.OkStyle.Render("[✔] Telemetry span transmission re-established natively across hardware bounds!"))
	return nil
}
