package runtime

import (
	"encoding/json"
	"github.com/spf13/cobra"
)

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

type CommandMetadata struct {
	Capability          CapabilityClass `json:"capability_class"`
	Risk                RiskLevel       `json:"risk_level"`
	IsIdempotent        bool            `json:"idempotent"`
	SupportsDryRun      bool            `json:"supports_dry_run"`
	RequiresInteractive bool            `json:"requires_interactive"`
	SchemaRef           string          `json:"schema_ref,omitempty"`
}

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
