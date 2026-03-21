package main

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/QuanuX/qxctl/cmd"
	qxerrors "github.com/QuanuX/qxctl/internal/errors"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/infra"
)

func main() {
	// Task 5: Live OS Signals
	ctx, cancel := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer cancel()

	// Parse rigid overrides organically (No Cobra coupling)
	ovr := runtime.ParseOverrides(os.Args[1:])

	app, err := runtime.New(ctx, ovr)
	if err != nil {
		qxerrors.Exit(err)
	}

	// Task B: Strictly passed injected dependencies.
	root := cmd.NewRootCmd(app)

	// Enforce global structured formatting for OS signals immediately
	infra.WatchTermination(ctx, app.Out)

	defer func() {
		if r := recover(); r != nil {
			err, ok := r.(error)
			if !ok {
				err = fmt.Errorf("panic: %v", r)
			}
			cliErr := qxerrors.New(qxerrors.CategoryInternalBug, "A native Go panic was trapped securely at the global execution boundary", err)
			app.Out.ErrorExit(cliErr)
		}
	}()

	if err := root.ExecuteContext(ctx); err != nil {
		app.Out.ErrorExit(err)
	}
}
