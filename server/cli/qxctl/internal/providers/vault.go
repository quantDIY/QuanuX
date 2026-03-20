package providers

import "context"

// SecretStore abstracts QuanuX Sovereign Vault and Identity policies seamlessly.
type SecretStore interface {
	Ping(ctx context.Context, token string) (bool, error)
	Read(ctx context.Context, path string) (map[string]interface{}, error)
}

func DefaultSecretStore() SecretStore {
	return &PhysicalSecretStore{}
}

type PhysicalSecretStore struct {
}

func (s *PhysicalSecretStore) Ping(ctx context.Context, token string) (bool, error) {
	return true, nil
}

func (s *PhysicalSecretStore) Read(ctx context.Context, path string) (map[string]interface{}, error) {
	return map[string]interface{}{}, nil
}
