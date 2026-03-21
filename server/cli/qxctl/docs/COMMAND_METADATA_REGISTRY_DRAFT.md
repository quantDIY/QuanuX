# Command Metadata Registry Spec (Patch-Planning)

**[CURRENT TRUTH]** `qxctl` mounts 34 Cobra commands. No structured metadata exists beyond `Use` and `Short` strings.
**[PROPOSAL]** Introduce a strict, statically typed registry binding struct metadata to Cobra's native `Annotations` map.

## 1. Target Files
- **New File:** `server/cli/qxctl/internal/runtime/metadata.go`
- **Modified File:** `server/cli/qxctl/cmd/root_test.go`
- **Modified Files:** All 34 `server/cli/qxctl/cmd/*.go` constructors.

## 2. Proposed Go Type Definitions (`metadata.go`)

```go
package runtime

// Enum mappings for structural integrity.
type CapabilityClass string
const (
	CapObserve        CapabilityClass = "observe"
	CapInspect        CapabilityClass = "inspect"
	CapValidate       CapabilityClass = "validate"
	CapSimulate       CapabilityClass = "simulate"
	CapDeploy         CapabilityClass = "deploy"
	CapMutate         CapabilityClass = "mutate"
	CapDestroy        CapabilityClass = "destroy"
	CapSecretsRead    CapabilityClass = "secrets-read"
	CapSecretsWrite   CapabilityClass = "secrets-write"
	CapInfraAdmin     CapabilityClass = "infra-admin"
	CapExecutionAdmin CapabilityClass = "execution-admin"
)

type RiskLevel string
const (
	RiskStable         RiskLevel = "stable"
	RiskExperimental   RiskLevel = "experimental"
	RiskLaunchCritical RiskLevel = "launch-critical"
	RiskDangerous      RiskLevel = "dangerous"
)

// CommandMetadata encapsulates the AI contract.
type CommandMetadata struct {
	Capability          CapabilityClass `json:"capability_class"`
	Risk                RiskLevel       `json:"risk_level"`
	IsIdempotent        bool            `json:"idempotent"`
	SupportsDryRun      bool            `json:"supports_dry_run"`
	RequiresInteractive bool            `json:"requires_interactive"`
	SchemaRef           string          `json:"schema_ref,omitempty"`
}
```

## 3. Metadata Binding Strategy

**[PROPOSAL]** Use a native wrapper to inject the JSON into Cobra's map.

```go
import (
	"encoding/json"
	"github.com/spf13/cobra"
)

// BindMetadata natively injects the contract without modifying Cobra.
func BindMetadata(cmd *cobra.Command, meta CommandMetadata) {
	if cmd.Annotations == nil {
		cmd.Annotations = make(map[string]string)
	}
	bytes, err := json.Marshal(meta)
	if err != nil {
		panic("Failed to encode command metadata statically: " + err.Error())
	}
	cmd.Annotations["qxctl_metadata"] = string(bytes)
}
```

## 4. Validation Strategy (Startup / Test Time)

**[PROPOSAL]** Do not incur reflection costs at runtime. Enforce mapping inside `go test`.
Modify `server/cli/qxctl/cmd/root_test.go`:

```go
func TestCommandMetadataCompleteness(t *testing.T) {
	app, _ := runtime.New(context.Background(), runtime.Overrides{})
	root := NewRootCmd(app)

	var walk func(*cobra.Command)
	walk = func(c *cobra.Command) {
		if c.RunE != nil || c.Run != nil { // Only leaves require metadata
			if _, ok := c.Annotations["qxctl_metadata"]; !ok {
				t.Errorf("Command %q is missing CommandMetadata binding", c.CommandPath())
			}
		}
		for _, child := range c.Commands() {
			walk(child)
		}
	}
	walk(root)
}
```

## 5. Derivation Path for AI / Docs

**[PROPOSAL]** 
- **Docs:** Read `cmd.Annotations["qxctl_metadata"]` inside the Cobra markdown generator template.
- **Manifest Generator:** Iterates `rootCmd.Commands()`, extracting the JSON strings and appending to a flat JSON Array natively.

## 6. Worked Examples (Current Verified Commands)

### A. `server/cli/qxctl/cmd/engine.go` (engine start)
**[CURRENT TRUTH]** Deploys pipelines over NATS. Non-idempotent.
**[PROPOSAL]**
```go
startCmd := &cobra.Command{ Use: "start" }
runtime.BindMetadata(startCmd, runtime.CommandMetadata{
	Capability:          runtime.CapExecutionAdmin,
	Risk:                runtime.RiskLaunchCritical,
	IsIdempotent:        false,
	SupportsDryRun:      false,
	RequiresInteractive: false,
})
```

### B. `server/cli/qxctl/cmd/secrets.go` (secrets setup)
**[CURRENT TRUTH]** Invokes Charmbracelet interactive loop.
**[PROPOSAL]**
```go
setupCmd := &cobra.Command{ Use: "setup" }
runtime.BindMetadata(setupCmd, runtime.CommandMetadata{
	Capability:          runtime.CapSecretsWrite,
	Risk:                runtime.RiskLaunchCritical,
	IsIdempotent:        false,
	SupportsDryRun:      false,
	RequiresInteractive: true,
})
```

### C. `server/cli/qxctl/cmd/query.go` (query validate)
**[CURRENT TRUTH]** Merely parses AST strings on disk.
**[PROPOSAL]**
```go
valCmd := &cobra.Command{ Use: "validate" }
runtime.BindMetadata(valCmd, runtime.CommandMetadata{
	Capability:          runtime.CapValidate,
	Risk:                runtime.RiskStable,
	IsIdempotent:        true,
	SupportsDryRun:      false,
	RequiresInteractive: false,
})
```
