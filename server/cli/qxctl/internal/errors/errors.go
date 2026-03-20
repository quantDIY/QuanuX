package errors

import (
	"fmt"
	"os"

	"github.com/QuanuX/qxctl/internal/theme"
)

type ErrorCategory int

const (
	CategoryUsage ErrorCategory = iota + 1
	CategoryAuth
	CategoryConnectivity
	CategoryTransient
	CategoryInternalBug
)

type CliError struct {
	Category ErrorCategory
	Message  string
	Err      error
}

func (e *CliError) Error() string {
	if e.Err != nil {
		return fmt.Sprintf("[%d] %s: %v", e.Category, e.Message, e.Err)
	}
	return fmt.Sprintf("[%d] %s", e.Category, e.Message)
}

func New(category ErrorCategory, message string, err error) *CliError {
	return &CliError{
		Category: category,
		Message:  message,
		Err:      err,
	}
}

// Exit natively halts the CLI with a deterministic UNIX error code bound to the domain.
func Exit(err error) {
	if err == nil {
		os.Exit(0)
	}

	if cliErr, ok := err.(*CliError); ok {
		fmt.Println(theme.FailStyle.Render(fmt.Sprintf("\nCommand Failed: %s\nCategory: %d", cliErr.Message, cliErr.Category)))
		if cliErr.Err != nil {
			fmt.Println(theme.DetailStyle.Render(fmt.Sprintf("Underlying error: %v", cliErr.Err)))
		}
		os.Exit(int(cliErr.Category))
	}

	fmt.Println(theme.FailStyle.Render(fmt.Sprintf("\nFatal System Error: %v", err)))
	os.Exit(99)
}
