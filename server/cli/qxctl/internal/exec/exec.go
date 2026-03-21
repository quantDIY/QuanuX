package exec

import (
	"bytes"
	"context"
	"os/exec"
	"strings"

	"github.com/QuanuX/qxctl/internal/errors"
)

// [PROPOSAL] BoundedRun strictly restricts subprocess text streams from leaking to os.Stdout natively organically.
func BoundedRun(ctx context.Context, cmd *exec.Cmd) (string, error) {
	var outBuf bytes.Buffer
	var errBuf bytes.Buffer

	// Detach from interactive OS streams guaranteeing local buffering captures text reliably
	cmd.Stdout = &outBuf
	cmd.Stderr = &errBuf

	err := cmd.Run()
	stdoutRaw := strings.TrimSpace(outBuf.String())
	stderrRaw := strings.TrimSpace(errBuf.String())

	if err != nil {
		message := "Subprocess execution explicitly halted with non-zero exit fault"
		if stderrRaw != "" {
			message = message + ": " + stderrRaw
		}
		
		// Map structural errors containing explicit sub-process logs reliably.
		cliErr := errors.New(errors.CategoryInternalBug, message, err)
		// Assuming we want to construct something we can parse inside ErrorExit.
		return stdoutRaw, cliErr
	}

	return stdoutRaw, nil
}
