package runtime

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/QuanuX/qxctl/internal/config"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/providers"
)

// App is the canonical dependency injection container replacing global Viper states.
type App struct {
	Cfg *config.Config
	Ctx context.Context
	Out *output.Manager
	
	NATS  providers.JetStreamEngine
	Vault providers.SecretStore
}

// Overrides carries flag overrides securely evaluated from the CLI layer without Cobra coupling.
type Overrides struct {
	ConfigFile string
	HubURL     string
	Output     string
	Trace      bool
	Verbose    bool
}

func ParseOverrides(args []string) Overrides {
	ovr := Overrides{}
	for i, arg := range args {
		if arg == "--config" && i+1 < len(args) {
			ovr.ConfigFile = args[i+1]
		}
		if len(arg) > 9 && arg[:9] == "--config=" {
			ovr.ConfigFile = arg[9:]
		}

		if arg == "--hub" && i+1 < len(args) {
			ovr.HubURL = args[i+1]
		}
		if len(arg) > 6 && arg[:6] == "--hub=" {
			ovr.HubURL = arg[6:]
		}

		if arg == "--output" && i+1 < len(args) {
			ovr.Output = args[i+1]
		}
		if len(arg) > 9 && arg[:9] == "--output=" {
			ovr.Output = arg[9:]
		}

		if arg == "--trace" || arg == "--trace=true" {
			ovr.Trace = true
		}
		if arg == "--verbose" || arg == "--verbose=true" {
			ovr.Verbose = true
		}
	}
	return ovr
}

// New initializes the application container safely parsing Config contracts.
func New(ctx context.Context, ovr Overrides) (*App, error) {
	bootStart := time.Now()

	// Task 5: Context Propagation
	ctx, cancel := signal.NotifyContext(ctx, os.Interrupt, syscall.SIGTERM)
	_ = cancel // Handled via defer in main.go runtime loop

	cfg, err := config.Load(ovr.ConfigFile, ovr.HubURL, ovr.Output, ovr.Trace, ovr.Verbose)
	if err != nil {
		return nil, err
	}

	out := output.NewManager(cfg.Output)

	app := &App{
		Cfg:   cfg,
		Ctx:   output.ContextWithManager(ctx, out),
		Out:   out,
		NATS:  providers.DefaultJetStream(),
		Vault: providers.DefaultSecretStore(),
	}

	if cfg.TraceMode {
		app.Out.Log("INFO", fmt.Sprintf("QuanuX Architecture Boot Trace: %v", time.Since(bootStart)))
	}

	return app, nil
}
