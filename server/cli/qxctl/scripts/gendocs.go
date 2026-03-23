package main

import (
	"context"
	"log"
	"os"

	"github.com/QuanuX/qxctl/cmd"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra/doc"
)

func main() {
	ovr := runtime.ParseOverrides([]string{})
	app, err := runtime.New(context.Background(), ovr)
	if err != nil {
		log.Fatalf("App init failed: %v", err)
	}
	rootCmd := cmd.NewRootCmd(app)

	// Suppress the date output in docs for stable diffs if needed, or just let it generate natively
	rootCmd.DisableAutoGenTag = true

	// Ensure directories exist natively
	os.MkdirAll("docs/man", 0755)
	os.MkdirAll("docs/md", 0755)

	header := &doc.GenManHeader{
		Title:   "QXCTL",
		Section: "1",
		Source:  "QuanuX Hardware Bounds",
		Manual:  "QuanuX Operator Manual",
	}

	err = doc.GenManTree(rootCmd, header, "docs/man")
	if err != nil {
		log.Fatalf("Fatal error generating man pages: %v", err)
	}

	err = doc.GenMarkdownTree(rootCmd, "docs/md")
	if err != nil {
		log.Fatalf("Fatal error generating markdown tree: %v", err)
	}

	log.Println("qxctl Machine-Assisted Documentation generated successfully into docs/man and docs/md.")
}
