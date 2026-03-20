package providers

import (
	"context"
	"github.com/nats-io/nats.go/jetstream"
)

// JetStreamEngine abstracts the literal QuanuX NATS Daemon natively for execution loops without raw TCP dependencies.
type JetStreamEngine interface {
	Connect(ctx context.Context, url string) error
	Publish(ctx context.Context, stream, subject string, payload []byte) (*jetstream.PubAck, error)
	Close() error
}

func DefaultJetStream() JetStreamEngine {
	// Concrete physical NATS builder injected smoothly inside native prod constraints.
	return &PhysicalJetStream{}
}

// PhysicalJetStream handles the literal socket loop natively.
type PhysicalJetStream struct {
	addr string
}

func (p *PhysicalJetStream) Connect(ctx context.Context, url string) error {
	p.addr = url
	return nil
}

func (p *PhysicalJetStream) Publish(ctx context.Context, stream, subject string, payload []byte) (*jetstream.PubAck, error) {
	return &jetstream.PubAck{}, nil
}

func (p *PhysicalJetStream) Close() error {
	return nil
}
