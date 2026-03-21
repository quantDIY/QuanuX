package exec

import (
	"bytes"
	"context"
	"os"
	"os/exec"
	"sync"

	"github.com/QuanuX/qxctl/internal/errors"
)

type BoundedOutput struct {
	Truncated     bool   `json:"truncated"`
	CapturedBytes int    `json:"captured_bytes"`
	SpillPath     string `json:"spill_path"`
	StderrPresent bool   `json:"stderr_present"`

	mu       sync.Mutex   `json:"-"`
	maxBytes int          `json:"-"`
	memBuf   bytes.Buffer `json:"-"`
	fileBuf  *os.File     `json:"-"`
}

func (b *BoundedOutput) write(p []byte, isErr bool) (int, error) {
	b.mu.Lock()
	defer b.mu.Unlock()

	if isErr && len(p) > 0 {
		b.StderrPresent = true
	}

	b.CapturedBytes += len(p)

	if b.CapturedBytes > b.maxBytes {
		if !b.Truncated {
			b.Truncated = true
			tmp, _ := os.CreateTemp("", "qxctl-spill-*.log")
			if tmp != nil {
				b.fileBuf = tmp
				b.SpillPath = tmp.Name()
				b.fileBuf.Write(b.memBuf.Bytes())
			}
			b.memBuf.Reset()
		}
		if b.fileBuf != nil {
			b.fileBuf.Write(p)
		}
	} else {
		b.memBuf.Write(p)
	}
	return len(p), nil
}

func (b *BoundedOutput) Close() {
	if b.fileBuf != nil {
		b.fileBuf.Close()
	}
}

type streamWriter struct {
	isErr bool
	b     *BoundedOutput
}

func (s *streamWriter) Write(p []byte) (int, error) {
	return s.b.write(p, s.isErr)
}

// BoundedRun strictly restricts subprocess text streams from leaking natively and initiates disk spilling upon OOM bounds.
func BoundedRun(ctx context.Context, cmd *exec.Cmd) (*BoundedOutput, error) {
	out := &BoundedOutput{
		maxBytes: 1024 * 1024, // 1MB limits
	}

	cmd.Stdout = &streamWriter{isErr: false, b: out}
	cmd.Stderr = &streamWriter{isErr: true, b: out}

	err := cmd.Run()
	out.Close()

	if err != nil {
		message := "Subprocess execution explicitly halted with non-zero exit fault"
		if out.StderrPresent {
			message = message + " (stderr present)"
		}
		cliErr := errors.New(errors.CategoryInternalBug, message, err)
		return out, cliErr
	}

	return out, nil
}
