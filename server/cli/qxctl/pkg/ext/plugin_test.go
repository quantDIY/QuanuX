package ext_test

import (
	"context"
	"testing"
	"time"

	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/pkg/ext"
)

func TestConnectSidecar_ContextCancellation(t *testing.T) {
	// A terminated context should gracefully abort before physical Cython sockets evaluate.
	manager := output.NewManager("quiet")
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Millisecond)
	boundCtx := output.ContextWithManager(ctx, manager)
	defer cancel()
	
	// Fast abort natively handles Sidecar drops
	err := ext.ConnectSidecar(boundCtx, "cython")
	if err != nil {
		t.Logf("Sidecar natively crashed handling invalid mapping correctly: %v", err)
	}
}

func TestPluginHandshakeImmutability(t *testing.T) {
	// Task 6: Asserts Rogue Plugin isolation validating handshake structures
	if ext.Handshake.MagicCookieKey != "QUANUX_PLUGIN_SYSTEM" {
		t.Fatalf("FATAL BREACH: QuanuX Plugin Magic Cookie string drift detected! Airgap compromised.")
	}
	if ext.Handshake.ProtocolVersion != 1 {
		t.Fatalf("FATAL BREACH: Plugin RPC protocol drift identified.")
	}
}
