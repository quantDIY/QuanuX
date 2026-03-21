package main

import (
	"context"
	"os"
	"os/signal"
	"syscall"

	"github.com/QuanuX/qxctl/cmd"
	qxerrors "github.com/QuanuX/qxctl/internal/errors"
	"github.com/QuanuX/qxctl/internal/runtime"
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

	if err := root.ExecuteContext(ctx); err != nil {
		app.Out.ErrorExit(err)
	}
}
