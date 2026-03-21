package output

type OutputStatus string

const (
	StatusSuccess OutputStatus = "success"
	StatusPartial OutputStatus = "partial"
	StatusError   OutputStatus = "error"
)

type OutputEnvelope struct {
	Status  OutputStatus `json:"status"`
	Code    int          `json:"exit_code"`
	Command string       `json:"command,omitempty"`
	Data    any          `json:"data,omitempty"`
	Message string       `json:"message,omitempty"`
	Error   *ErrorDetail `json:"error,omitempty"`
}

type ErrorDetail struct {
	Type          string `json:"type"`
	Message       string `json:"message"`
	SubProcessLog string `json:"sub_process_log,omitempty"`
}
