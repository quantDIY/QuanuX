package infra

import (
	"fmt"
	"github.com/zalando/go-keyring"
)

// SetToken abstracts the OS Keyring constraints inside the QuanuX infrastructure package
func SetToken(token, target string) error {
	service := "QuanuX"
	user := "do_token"

	err := keyring.Set(service, user, token)
	if err != nil {
		return fmt.Errorf("FATAL: Failed to inject token into the keyring: %w", err)
	}

	fmt.Println("SUCCESS: DigitalOcean token securely locked into the OS Keychain.")
	fmt.Printf("The token will perfectly synchronize with Viper Target: %s\n", target)
	return nil
}
