package nats

import (
	"encoding/json"
	"fmt"
	"time"

	"github.com/QuanuX/QuanuX/execution-node/internal/config"
	"github.com/nats-io/nats.go"
)

type Client struct {
	config *config.NodeConfig
	nc     *nats.Conn
}

func NewClient(cfg *config.NodeConfig) *Client {
	return &Client{
		config: cfg,
	}
}

func (c *Client) Connect() error {
	// Connect to the LOCAL Leaf Node, not the remote hub directly.
	url := fmt.Sprintf("nats://localhost:%d", c.config.LeafPort)

	// Retry logic
	opts := []nats.Option{
		nats.Name(c.config.FriendlyName),
		nats.ReconnectWait(2 * time.Second),
		nats.MaxReconnects(-1), // Infinite
		nats.DisconnectErrHandler(func(nc *nats.Conn, err error) {
			fmt.Printf("Disconnected from Local Leaf: %v\n", err)
		}),
		nats.ReconnectHandler(func(nc *nats.Conn) {
			fmt.Printf("Reconnected to Local Leaf [%s]\n", nc.ConnectedUrl())
		}),
	}

	nc, err := nats.Connect(url, opts...)
	if err != nil {
		return err
	}

	c.nc = nc
	return nil
}

func (c *Client) Close() {
	if c.nc != nil {
		c.nc.Close()
	}
}

type Heartbeat struct {
	NodeID    string            `json:"node_id"`
	Name      string            `json:"name"`
	Timestamp int64             `json:"timestamp"`
	Tags      []string          `json:"tags"`
	Stats     map[string]string `json:"stats"` // cpu, mem, etc (placeholder)
}

func (c *Client) PublishHeartbeat() error {
	if c.nc == nil {
		return fmt.Errorf("nats client not connected")
	}

	hb := Heartbeat{
		NodeID:    c.config.NodeID,
		Name:      c.config.FriendlyName,
		Timestamp: time.Now().UnixMilli(),
		Tags:      []string{"proto", "go", "bare-metal"},
		Stats:     map[string]string{"status": "ok"},
	}

	data, err := json.Marshal(hb)
	if err != nil {
		return err
	}

	subject := fmt.Sprintf("node.%s.heartbeat", c.config.NodeID)
	return c.nc.Publish(subject, data)
}
