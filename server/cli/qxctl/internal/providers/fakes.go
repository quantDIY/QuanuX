package providers

import (
	"context"
	"github.com/nats-io/nats.go/jetstream"
)

// FakeJetStream guarantees 100% CI coverage isolating network bindings cleanly.
type FakeJetStream struct {
	PublishCalls int
	SubjectTrace string
}

func (f *FakeJetStream) Connect(ctx context.Context, url string) error { return nil }
func (f *FakeJetStream) Publish(ctx context.Context, stream, subject string, payload []byte) (*jetstream.PubAck, error) {
	f.PublishCalls++
	f.SubjectTrace = subject
	return &jetstream.PubAck{Duplicate: false}, nil
}
func (f *FakeJetStream) Close() error { return nil }

// FakeSecretStore tests native encryption routes validating JSON keys without literal Vault servers.
type FakeSecretStore struct {
	Sealed bool
}

func (f *FakeSecretStore) Ping(ctx context.Context, token string) (bool, error) {
	return !f.Sealed, nil
}
func (f *FakeSecretStore) Read(ctx context.Context, path string) (map[string]interface{}, error) {
	return map[string]interface{}{"identity": "golden-fake-uid", "role": "operator"}, nil
}
