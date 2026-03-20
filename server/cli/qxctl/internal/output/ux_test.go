package output_test

import (
	"bytes"
	"context"
	"io"
	"os"
	"strings"
	"testing"

	"github.com/QuanuX/qxctl/internal/output"
)

func TestLiveRedaction(t *testing.T) {
	// Task D Verification: Assert strings pass completely through the scrubber directly.
	manager := output.NewManager("text")
	
	// Hijack os.Stdout cleanly
	r, w, _ := os.Pipe()
	oldStdout := os.Stdout
	os.Stdout = w

	// Emit raw string with a secret NATS token directly natively.
	manager.EmitRaw("Connecting natively with token = 'super-secret-bearer-99' over TCP.")
	
	// Close pipe
	w.Close()
	os.Stdout = oldStdout

	var buf bytes.Buffer
	io.Copy(&buf, r)
	out := buf.String()

	if strings.Contains(out, "super-secret-bearer-99") {
		t.Fatalf("Redaction FAILURE: Raw token leaked directly into STDOUT buffers! Output: %v", out)
	}

	if !strings.Contains(out, "[REDACTED]") {
		t.Fatalf("Redaction MISS: Scrubber missed masking the payload signature.")
	}
}

func TestContextBinding(t *testing.T) {
	// Validate isolation
	ctx := context.Background()
	manager := output.NewManager("json")
	
	boundCtx := output.ContextWithManager(ctx, manager)
	retrieved := output.FromContext(boundCtx)
	
	if retrieved.Mode != "json" {
		t.Fatalf("Dependency Injection FAILURE: Expected 'json' context loop natively.")
	}
}
