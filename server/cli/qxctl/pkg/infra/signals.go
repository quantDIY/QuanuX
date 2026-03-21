package infra

import (
	"context"
	"fmt"

	"github.com/QuanuX/qxctl/internal/errors"
	"github.com/QuanuX/qxctl/internal/output"
)

// WatchTermination executes actively in the background enforcing structured JSON emission
// upon receiving OS-level interruption bounds (e.g. SIGINT/SIGTERM), preventing dirty textual exits.
func WatchTermination(ctx context.Context, out *output.Manager) {
	go func() {
		<-ctx.Done()
		if ctx.Err() == context.Canceled {
			out.ErrorExit(errors.New(errors.CategoryInternalBug, "Execution terminated by OS signal natively", fmt.Errorf("SIGINT/SIGTERM received")))
		}
	}()
}
