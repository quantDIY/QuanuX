# Capability Enforcement Insertion Plan (Patch-Planning)

**[CURRENT TRUTH]** The CLI assumes user execution environment absolute authority natively. `runtime.App` securely provisions dependencies but executes no bound checking.
**[PROPOSAL]** Inject an Authorization hook mapped via Cobra's `PreRunE` bound structurally to the new `CommandMetadata`.

## 1. Target Files
- **Modified File:** `server/cli/qxctl/internal/runtime/app.go`
- **Modified Files:** All 34 `server/cli/qxctl/cmd/*.go` constructors.

## 2. `runtime.App` Structure Updates

**[CURRENT TRUTH]**
```go
type App struct {
	Cfg   *config.Config
	Ctx   context.Context
	Out   *output.Manager
	NATS  providers.JetStreamEngine
	Vault providers.SecretStore
}
```

**[PROPOSAL - EXACT ADDITIONS]**
```go
type App struct {
	// ... (Current Truth maps)
	Session *security.SessionToken // New structural AI context
}

// In internal/security/session.go
type SessionToken struct {
	AgentID       string
	Capabilities  []string // e.g. []string{"inspect", "validate"}
}
```

*(Note: If `SessionToken` cannot be cryptographically sourced initially, we simulate it via an OS environment token `$QUANUX_AI_CAPABILITIES` strictly for the V1 AI loop.)*

## 3. Composition Strategy (`PreRunE`)

**[PROPOSAL]** Tie the enforcement hook strictly to the `qxctl_metadata` string attached during construction. Do NOT pass hardcoded strings to `app.Authorize()`. Let Cobra's context govern it dynamically.

```go
// In internal/runtime/auth.go
func (app *App) AuthorizeAndPrepare() func(cmd *cobra.Command, args []string) error {
	return func(cmd *cobra.Command, args []string) error {
		// 1. Extract the bound Metadata natively.
		metaStr, ok := cmd.Annotations["qxctl_metadata"]
		if !ok {
			app.Out.ErrorExit(errors.New("Unmapped bounds"), 1, "SYSTEM_ERROR", "")
		}

		var meta CommandMetadata
		json.Unmarshal([]byte(metaStr), &meta)

		// 2. Dry-Run Interaction Rule
		reqCap := meta.Capability
		dryRun, _ := cmd.Flags().GetBool("dry-run")
		if dryRun && meta.SupportsDryRun {
			// Downgrade purely mutational commands when simulating securely natively.
			if reqCap == CapDeploy || reqCap == CapMutate {
				reqCap = CapSimulate
			}
		}

		// 3. Evaluate Agent Authority Ring
		if !app.Session.HasCapability(reqCap) {
			// Denial Flow dynamically routed directly back into the structurally strict Error boundary.
			err := fmt.Errorf("Agent lacks %s capability", reqCap)
			app.Out.ErrorExit(err, 5, "AUTHORIZATION_DENIED", "")
		}

		return nil
	}
}
```

## 4. Modified Command Constructors

**[PROPOSAL]** Instead of passing parameters per command, Cobra invokes `app.AuthorizeAndPrepare()` consistently universally natively.

```go
// Inside cmd/vault.go
func NewVaultCmd(app *runtime.App) *cobra.Command {
	statusCmd := &cobra.Command{
		Use:   "status",
		Short: "Interrogates C++ Annex storage",
		// PreRunE hooks identically for every single command natively.
		PreRunE: app.AuthorizeAndPrepare(),
		RunE: func(cmd *cobra.Command, args []string) error {
			return vault.Status(app.Ctx)
		},
	}
	
	// Binding attaches the data evaluated by PreRunE dynamically.
	runtime.BindMetadata(statusCmd, runtime.CommandMetadata{
		Capability: runtime.CapInspect,
		Risk:       runtime.RiskStable,
	})
	
	return statusCmd
}
```

## 5. Mapped Capabilities (5 Current Verified Commands)

1. `server/cli/qxctl/cmd/infra.go` -> `infra apply` 
   - **Capability:** `CapInfraAdmin`
   - **Note:** Highest structural physical authority.
2. `server/cli/qxctl/cmd/vault.go` -> `vault status`
   - **Capability:** `CapInspect`
   - **Note:** Passive polling over local socket bounds.
3. `server/cli/qxctl/cmd/secrets.go` -> `secrets get`
   - **Capability:** `CapSecretsRead`
   - **Note:** Requires Ring cryptography bindings natively.
4. `server/cli/qxctl/cmd/spreader.go` -> `spreader package`
   - **Capability:** `CapSimulate`
   - **Note:** Merely executes local C++ compiler pipelines against payload.
5. `server/cli/qxctl/cmd/node.go` -> `node deploy`
   - **Capability:** `CapDeploy`
   - **Note:** Requires SSH drops into production nodes natively.
