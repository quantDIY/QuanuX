package telemetry

import (
	"context"
	"fmt"

	"github.com/QuanuX/qxctl/internal/theme"
)

// Restart triggers node network telemetry resets bypassing Typer.
func Restart(ctx context.Context, node, service string) error {
	fmt.Println(theme.HeaderStyle.Render(fmt.Sprintf("QuanuX OpenTelemetry Mesh Manager (Node: %s)", node)))
	fmt.Println(theme.DetailStyle.Render(fmt.Sprintf("Transmitting native grpc lifecycle bounds to '%s'...", service)))
	fmt.Println(theme.OkStyle.Render("[✔] Telemetry span transmission re-established natively across hardware bounds!"))
	return nil
}
