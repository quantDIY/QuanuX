package cmd_test

import (
	"context"
	"os"
	"strings"
	"testing"

	"github.com/QuanuX/qxctl/cmd"
	"github.com/QuanuX/qxctl/internal/config"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/providers"
	"github.com/QuanuX/qxctl/internal/runtime"
)

func TestCensusParity(t *testing.T) {
	out := output.NewManager("quiet")
	app := &runtime.App{
		Cfg:   &config.Config{},
		Ctx:   output.ContextWithManager(context.Background(), out),
		Out:   out,
		NATS:  &providers.FakeJetStream{},
		Vault: &providers.FakeSecretStore{},
	}

	root := cmd.NewRootCmd(app)
	cmds := root.Commands()

	// Enforce 34 mathematical command tree limit natively
	if len(cmds) != 34 {
		t.Fatalf("CRITICAL DRIFT: Root CMD mounts %d commands dynamically, expected exactly 34. The structural boundary has evolved without architectural compliance updates.", len(cmds))
	}

	contentBytes, err := os.ReadFile("../COMMAND_CENSUS.md")
	if err != nil {
		t.Fatalf("Failed to load census artifact: %v", err)
	}
	content := string(contentBytes)

	// Enforce 1-to-1 parity mapping the runtime tree directly to the Markdown matrix organically
	for _, c := range cmds {
		name := c.Name()
		if !strings.Contains(content, "`"+name+"`") {
			t.Fatalf("CENSUS DRIFT: Command '%s' is physically mounted but missing from COMMAND_CENSUS.md tracking matrix.", name)
		}
	}
}
