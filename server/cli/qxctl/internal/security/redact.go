package security

import (
	"regexp"
)

var (
	// Task 10: Codify Security Enforcements. Redact Vault keys, NATS passwords, and API Tokens natively.
	secretRegexes = []*regexp.Regexp{
		// Match Bearer tokens: Prefix = "Bearer ", Token = "ABC...", Suffix = ""
		regexp.MustCompile(`(?i)(bearer\s+)([A-Za-z0-9\-\._~+/]+=*)()`),
		
		// Match Key/Value tokens: Prefix = `token = '`, Token = `...`, Suffix = `'`
		regexp.MustCompile(`(?i)(["']?(?:password|token|secret|key)["']?\s*[:=]\s*["']?)([^"'\s]+)(["']?)`),
		
		// Match Vault signatures: Prefix = ``, Token = `s.123...`, Suffix = ``
		regexp.MustCompile(`()(s\.[A-Za-z0-9]{24})()`),
	}
)

// Redact explicitly scrubs standard string outputs preventing Trust Boundary leakage.
func Redact(input string) string {
	scrubbed := input
	for _, re := range secretRegexes {
		// By enforcing exactly 3 capture groups globally: (Prefix)(Token)(Suffix)
		// ${1} perfectly retains spacing constraints while ${3} saves boundary quotes organically.
		scrubbed = re.ReplaceAllString(scrubbed, "${1}[REDACTED]${3}")
	}
	return scrubbed
}
