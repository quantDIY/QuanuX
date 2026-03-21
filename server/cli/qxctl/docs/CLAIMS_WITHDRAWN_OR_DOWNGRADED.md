# Claims Withdrawn or Downgraded

A forensic audit of the previous `qxctl` documentation pack mandates the explicit correction, downgrading, or withdrawal of several speculative assertions.

### 1. The "34 Commands" Claim
- **Original Claim**: "`qxctl` mounts exactly 34 primary commands..."
- **Why it was overstated**: The number `34` refers rigidly to the literal number of `cmd/X.go` constructor configurations mapped inside `rootCmd.AddCommand()`. The actual number of functional *leaf* subcommands is 58.
- **Corrected Statement**: "The binary constructs 34 top-level command packages, resulting natively in 58 functional leaf commands."
- **Status Mapping**: [STRONG INFERENCE] updated to [VERIFIED].

### 2. The Existence of `qxctl cluster manifest`
- **Original Claim**: "The manifest should be automatically generated... exposed via `qxctl cluster manifest --output=json`."
- **Why it was unsupported**: The language loosely implied the structure might already exist.
- **Corrected Statement**: "`qxctl` possesses no functionality to emit metadata today natively. The command `manifest` must be structurally created."
- **Status Mapping**: Remainder explicitly marked [PROPOSAL]. 

### 3. Dry-Run Pervasiveness
- **Original Claim** (implied in gaps list): "Missing `--dry-run` bounds on memory-intense commands (e.g. spreader deploy)."
- **Why it was supported/unsupported**: The claim that `spreader deploy` lacks dry-run is [VERIFIED] accurate against the dump array. However, any assumption that `--dry-run` acts identically across commands is rejected. 
- **Corrected Statement**: "`--dry-run` exists exclusively on `infra gcp-sql execute` and `node deploy`. It is absent natively on `infra apply`, `nest drop`, and `spreader deploy`."
- **Status Mapping**: [VERIFIED].

### 4. Interactive UX Behavior
- **Original Claim**: "Interactive TUI Leakage: `secrets setup` and some deployment hooks invoke Text User Interfaces..."
- **Why it was unproven**: While `secrets setup` is proven interaction via `charmbracelet`, stating "and some deployment hooks" was an assumption without evidence.
- **Corrected Statement**: "The `secrets setup` interactive ring explicitly triggers TUI hazard blocks dynamically natively. No deployment hooks were observed doing the same in the tree dump natively."
- **Status Mapping**: Downgraded to [WEAK INFERENCE].

### 5. `runtime.App` Field Assumptions
- **Original Claim**: Speculating capability bindings or token interfaces.
- **Why it was unproven**: `runtime.App` strictly maps only 5 specific core struct endpoints.
- **Corrected Statement**: "`runtime.App` securely provisions JetStream, Config, UX strings, Context spans, and Vault rings. Security Capabilities do not trace natively."
- **Status Mapping**: Remainder strictly mapped [PROPOSAL].
