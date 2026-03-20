package security

import (
	"testing"
)

func TestRedactVaultToken(t *testing.T) {
	// Task 10: Prove invariant behavior with rigorous tests securely masking secrets.
	input := "Vault execution successful. Token: s.4A731nBdkk29d10kllP192mA received."
	expected := "Vault execution successful. Token: [REDACTED] received."

	result := Redact(input)
	if result != expected {
		t.Errorf("Security Redaction Failed. \nExpected: %s\nGot: %s", expected, result)
	}
}

func TestRedactBearerToken(t *testing.T) {
	input := "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.e30.123456"
	expected := "Authorization: Bearer [REDACTED]"

	result := Redact(input)
	if result != expected {
		t.Errorf("Security Redaction Failed. \nExpected: %s\nGot: %s", expected, result)
	}
}

func TestRedactPassword(t *testing.T) {
	input := `{"user": "admin", "password": "supersecretpassword123"}`
	expected := `{"user": "admin", "password": "[REDACTED]"}`

	result := Redact(input)
	if result != expected {
		t.Errorf("Security Redaction Failed. \nExpected: %s\nGot: %s", expected, result)
	}
}
