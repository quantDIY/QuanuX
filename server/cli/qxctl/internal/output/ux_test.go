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

	// Emit across ALL exposed formatter boundaries to brutally prove mask loops.
	manager.EmitRaw("Connecting natively with token = 'super-secret-bearer-99' over TCP.")
	manager.EmitRawf("Vault signature: %s\n", "s.1234567890abcdef12345678")
	manager.Info("bearer my-secret-oidc-string connected!")
	manager.Warn("Leaking password: 'db-secret-password-xyz'")
	manager.Error("Fatal Error. Key=s.abcdef1234567890abcdef12")
	manager.Debug("Token: super-secret-bearer-99 initialized")
	
	// Close pipe
	w.Close()
	os.Stdout = oldStdout

	var buf bytes.Buffer
	io.Copy(&buf, r)
	out := buf.String()

	leakChecks := []string{
		"super-secret-bearer-99", 
		"s.1234567890abcdef12345678", 
		"my-secret-oidc-string", 
		"db-secret-password-xyz",
		"s.abcdef1234567890abcdef12",
	}

	for _, leak := range leakChecks {
		if strings.Contains(out, leak) {
			t.Fatalf("Redaction FAILURE: Raw token leaked directly into STDOUT buffers! Output: %v", leak)
		}
	}

	if strings.Count(out, "[REDACTED]") < 6 {
		t.Fatalf("Redaction MISS: Scrubber mapping count failed. Expected at least 6 tokens neutralized. Output: %v", out)
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
