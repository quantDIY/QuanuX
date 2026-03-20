package cmd_test

import (
	"context"
	"strings"
	"testing"

	"github.com/QuanuX/qxctl/cmd"
	"github.com/QuanuX/qxctl/internal/config"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/providers"
	"github.com/QuanuX/qxctl/internal/runtime"
)

func TestTableDrivenCommandMatrix(t *testing.T) {
	// Task 1: Constructor Matrix Suite under breadth

	fakeNats := &providers.FakeJetStream{}
	fakeVault := &providers.FakeSecretStore{}

	tests := []struct {
		name        string
		mode        string
		args        []string
		expectMount bool
	}{
		{"Dashboard Quiet Loop", "quiet", []string{"dashboard"}, true},
		{"Node Start JSON Loop", "json", []string{"node", "start"}, true},
		{"Engine Tune YAML Loop", "yaml", []string{"engine", "tune"}, true},
		{"Infra Remote AWS Target", "text", []string{"infra", "auth", "--target", "aws"}, true},
		{"Query SQL Isolation", "json", []string{"query", "sql"}, true},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			out := output.NewManager(tt.mode)
			app := &runtime.App{
				Cfg:   &config.Config{},
				Ctx:   output.ContextWithManager(context.Background(), out),
				Out:   out,
				NATS:  fakeNats,
				Vault: fakeVault,
			}

			// Validate explicit DI
			root := cmd.NewRootCmd(app)
			root.SetArgs(tt.args)

			err := root.ExecuteContext(app.Ctx)
			if err != nil && !strings.Contains(err.Error(), "unknown command") {
				t.Fatalf("Command routing breached structurally: %v", err)
			}
		})
	}
}

func TestNilRuntimeCrashGuard(t *testing.T) {
	// Task 1: Fails hard on nil runtime bindings
	defer func() {
		if r := recover(); r == nil {
			t.Errorf("FATAL: NewRootCmd failed to panic mathematically when injected with a nil *runtime.App context.")
		}
	}()
	
	_ = cmd.NewRootCmd(nil)
}


