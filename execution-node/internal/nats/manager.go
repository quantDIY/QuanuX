package nats

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"syscall"
	"time"

	"github.com/QuanuX/QuanuX/execution-node/internal/config"
)

type Manager struct {
	config    *config.NodeConfig
	configDir string
	cmd       *exec.Cmd
}

func NewManager(cfg *config.NodeConfig) (*Manager, error) {
	dir, err := config.GetConfigDir()
	if err != nil {
		return nil, err
	}
	return &Manager{
		config:    cfg,
		configDir: dir,
	}, nil
}

func (m *Manager) GenerateConfig() error {
	if m.config.Hub == nil {
		return nil
	}

	confPath := filepath.Join(m.configDir, "nats-leaf.conf")

	// Ensure jetstream dir exists
	jsDir := filepath.Join(m.configDir, "jetstream")
	os.MkdirAll(jsDir, 0755)

	content := fmt.Sprintf(`
# QuanuX Node - NATS Leaf Node Configuration
port: %d

leafnodes {
    port: %d
    remotes [
        {
            url: "%s"
            # credentials: "%s/hub.creds" 
        }
    ]
}

jetstream {
    store_dir: "%s"
    max_mem: 1G
    max_file: 5G
}
`, m.config.NatsPort, m.config.LeafPort, m.config.Hub.URL, m.configDir, jsDir)

	return os.WriteFile(confPath, []byte(content), 0644)
}

func (m *Manager) Start() error {
	if err := m.GenerateConfig(); err != nil {
		return fmt.Errorf("failed to generate nats config: %w", err)
	}

	confPath := filepath.Join(m.configDir, "nats-leaf.conf")

	// check for nats-server binary
	_, err := exec.LookPath("nats-server")
	if err != nil {
		fmt.Println("Warning: 'nats-server' not found in PATH. Leaf node will not start.")
		return nil
	}

	fmt.Printf("Starting NATS Leaf Node (config: %s)...\n", confPath)
	m.cmd = exec.Command("nats-server", "-c", confPath)
	// Redirect stdout/stderr if needed, for now inherit
	m.cmd.Stdout = os.Stdout
	m.cmd.Stderr = os.Stderr

	if err := m.cmd.Start(); err != nil {
		return err
	}

	go func() {
		m.cmd.Wait()
	}()

	return nil
}

func (m *Manager) Stop() {
	if m.cmd != nil && m.cmd.Process != nil {
		fmt.Println("Stopping NATS Leaf Node...")
		m.cmd.Process.Signal(syscall.SIGTERM)

		// Wait a bit then kill
		done := make(chan error, 1)
		go func() {
			_, err := m.cmd.Process.Wait()
			done <- err
		}()

		select {
		case <-time.After(5 * time.Second):
			m.cmd.Process.Kill()
		case <-done:
			// exited
		}
	}
}
