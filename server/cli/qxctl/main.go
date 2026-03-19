package main

import (
	"log"

	"github.com/QuanuX/qxctl/cmd"
)

func main() {
	if err := cmd.Execute(); err != nil {
		log.Fatalf("Fatal error executing qxctl: %v", err)
	}
}
