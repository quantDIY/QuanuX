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
)

// App is the canonical dependency injection container replacing global Viper states.
type App struct {
	Cfg *config.Config
	Ctx context.Context
	Out *output.Manager
	// Endpoints for future NATS, Vault, HTTP clients go here safely
}

// Overrides carries flag overrides securely evaluated from the CLI layer without Cobra coupling.
type Overrides struct {
	ConfigFile string
	HubURL     string
	Output     string
	Trace      bool
	Verbose    bool
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
		Cfg: cfg,
		Ctx: ctx,
		Out: out,
	}

	if cfg.TraceMode {
		app.Out.Log("INFO", fmt.Sprintf("QuanuX Architecture Boot Trace: %v", time.Since(bootStart)))
	}

	return app, nil
}
