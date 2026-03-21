# Evidence Appendix: Redaction and Interaction

### 1. Redaction Patterns
**[VERIFIED]** Redaction actively screens strings before standard terminal emissions natively.
- **File:** `server/cli/qxctl/internal/security/redact.go` (Lines 9-18)
```go
var secretRegexes = []*regexp.Regexp{
	regexp.MustCompile(`(?i)(bearer\s+)([A-Za-z0-9\-\._~+/]+=*)()`),
	regexp.MustCompile(`(?i)(["']?(?:password|token|secret|key)["']?\s*[:=]\s*["']?)([^"'\s]+)(["']?)`),
	regexp.MustCompile(`()(s\.[A-Za-z0-9]{24})()`),
}
```

**[VERIFIED]** Only explicit internal wrapper calls invoke the engine realistically. By design, any nested library invoking `fmt.Fprintf(os.Stdout)` natively escapes the perimeter.
- **File:** `server/cli/qxctl/internal/output/ux.go` (Line 44)
```go
fmt.Println(security.Redact(rawStr))
```

### 2. Interactivity Proof (Correction)
**[PROPOSAL -> WEAK INFERENCE]** Previously, planning documents explicitly stated `qxctl secrets setup` generated interactive loops utilizing the `charmbracelet` framework which blocked autonomous AI behavior natively.

Upon strict forensic audit of `cmd/secrets.go`, the handler executes purely as an empty stub returning nil natively currently on the branch.
- **File:** `server/cli/qxctl/cmd/secrets.go` (Lines 36-38)
```go
setupCmd := &cobra.Command{
	Use:   "setup", Short: "Interactive wizard safely bound to outputs", RunE: func(cmd *cobra.Command, args []string) error { return nil },
}
```
**Conclusion:** There is **NO** verifiable evidence natively binding `charmbracelet` inside `secrets setup` today. The command is a conceptual placeholder. Any statements defining it as "AI Hostile" are withdrawn natively from the forensic analysis.
