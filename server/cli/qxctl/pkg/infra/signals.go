package infra

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/QuanuX/qxctl/internal/errors"
	"github.com/QuanuX/qxctl/internal/output"
)

// WatchTermination executes actively in the background enforcing structured JSON emission
// upon receiving OS-level interruption bounds (e.g. SIGINT/SIGTERM), preventing dirty textual exits.
func WatchTermination(out *output.Manager) {
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt, syscall.SIGTERM)
	go func() {
		s := <-sigChan
		out.ErrorExit(errors.New(errors.CategoryInternalBug, "Execution terminated by OS signal natively", fmt.Errorf("%v received", s)))
	}()
}
