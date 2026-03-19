package infra_test

import (
	"context"
	"testing"
	"github.com/QuanuX/qxctl/pkg/infra"
)

func TestApplyIsolation(t *testing.T) {
	ctx := context.Background()
	
	// Ensure that targeting an invalid or unauthorized folder returns an explicit isolated error natively
	err := infra.Apply(ctx, "invalid_target_should_fail")
	if err == nil {
		t.Fatal("Expected error when instructing Terraform API against invalid infra folder, got nil pointer")
	}
	
	t.Log("Successfully trapped bounding failure autonomously:", err)
}
