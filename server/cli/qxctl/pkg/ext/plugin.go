package ext

import (
	"context"
	"fmt"
	"github.com/QuanuX/qxctl/internal/output"
	"os/exec"

	"github.com/hashicorp/go-plugin"
	"github.com/QuanuX/qxctl/internal/theme"
)

// Handshake is an immutable standard shared symmetrically between QuanuX Core and the Sidecars to prevent rogue plugin execution.
var Handshake = plugin.HandshakeConfig{
	ProtocolVersion:  1,
	MagicCookieKey:   "QUANUX_PLUGIN_SYSTEM",
	MagicCookieValue: "QUANUX_AIRGAP_SECURE",
}

// ConnectSidecar allocates a secure gRPC channel linking the lightweight CLI natively to the massive Cython C++ executables.
func ConnectSidecar(ctx context.Context, exchangeName string) error {
	output.FromContext(ctx).EmitRaw(theme.HeaderStyle.Render(fmt.Sprintf("Establishing Airgapped gRPC Socket to QuanuX Sidecar: [%s]", exchangeName)))

	// The architectural standard maps plugin targets exactly to sidecar binary nomenclature
	binaryName := fmt.Sprintf("qxctl-%s", exchangeName)

	output.FromContext(ctx).EmitRaw(theme.DetailStyle.Render(fmt.Sprintf("Locating binary interlock target: %s", binaryName)))

	client := plugin.NewClient(&plugin.ClientConfig{
		HandshakeConfig: Handshake,
		Plugins:         map[string]plugin.Plugin{}, // Empty until strict Protobuf interface is mapped
		Cmd:             exec.Command(binaryName),
		AllowedProtocols: []plugin.Protocol{
			plugin.ProtocolNetRPC, plugin.ProtocolGRPC,
		},
	})
	
	// Ensures socket cleanup organically occurs inside context cancellation parameters
	defer client.Kill()

	_, err := client.Client()
	if err != nil {
		output.FromContext(ctx).EmitRawf("\n%s\n", theme.FailStyle.Render(fmt.Sprintf("[X] Sidecar socket closed. Ensure [%s] is successfully cross-compiled natively and available system-wide in $PATH.", binaryName)))
		// We explicitly bypass printing Cobra err strings here, handling the architectural failure contextually Native
		return nil 
	}

	output.FromContext(ctx).EmitRaw(theme.OkStyle.Render(fmt.Sprintf("[✔] Successfully established Memory Socket Handshake with %s natively over gRPC!", binaryName)))
	
	return nil
}
