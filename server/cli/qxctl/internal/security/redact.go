package security

import (
	"regexp"
)

var (
	// Task 10: Codify Security Enforcements. Redact Vault keys, NATS passwords, and API Tokens natively.
	secretRegexes = []*regexp.Regexp{
		regexp.MustCompile(`(?i)(bearer\s+)[A-Za-z0-9\-\._~+/]+=*`),
		regexp.MustCompile(`(?i)(["']?(password|token|secret|key)["']?\s*[:=]\s*["']?)[^"'\s]+(["']?)`),
		regexp.MustCompile(`(s\.[A-Za-z0-9]{24})`), // HashiCorp Vault Token signature
	}
)

// Redact explicitly scrubs standard string outputs preventing Trust Boundary leakage.
func Redact(input string) string {
	scrubbed := input
	for _, re := range secretRegexes {
		scrubbed = re.ReplaceAllString(scrubbed, "${1}[REDACTED]${3}")
	}
	return scrubbed
}
