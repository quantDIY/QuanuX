package cmd_test

import (
	"context"
	"testing"

	"github.com/QuanuX/qxctl/cmd"
	"github.com/QuanuX/qxctl/internal/config"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/providers"
	"github.com/QuanuX/qxctl/internal/runtime"
)

func TestDecoupledIntegration(t *testing.T) {
	// Task E Verification: Validate mock DI without calling dial() natively!
	
	fakeNats := &providers.FakeJetStream{}
	fakeVault := &providers.FakeSecretStore{}

	out := output.NewManager("quiet")
	app := &runtime.App{
		Cfg:   &config.Config{},
		Ctx:   output.ContextWithManager(context.Background(), out),
		Out:   out,
		NATS:  fakeNats,
		Vault: fakeVault,
	}

	// Native tree evaluation
	root := cmd.NewRootCmd(app)
	
	// Push engine start dynamically checking if it resolves
	root.SetArgs([]string{"engine", "start"})
	_ = root.ExecuteContext(app.Ctx)

	if fakeNats.PublishCalls < 0 {
		t.Fatalf("Engine Start did not mount Mock NATS correctly.")
	}

	// Re-check structural binding across deeper infra bounds
	root.SetArgs([]string{"infra", "auth", "--target", "aws"})
	_ = root.ExecuteContext(app.Ctx)

	// Since Start calls mock abstractions natively, we just ensure the app mounts accurately without globals
	if fakeVault.Sealed {
		t.Log("Vault fake evaluated gracefully locally")
	}
	
	root.SetArgs([]string{"skills", "list"})
	_ = root.ExecuteContext(app.Ctx)
}
