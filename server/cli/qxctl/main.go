package main

import (
	"context"
	"os"
	"os/signal"
	"syscall"

	"github.com/QuanuX/qxctl/cmd"
	qxerrors "github.com/QuanuX/qxctl/internal/errors"
)

func main() {
	// Task 5: Context Propagation
	// Physically wire interrupt barriers down into the QuanuX Go Context.
	ctx, cancel := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer cancel()

	if err := cmd.Execute(ctx); err != nil {
		qxerrors.Exit(err)
	}
}
