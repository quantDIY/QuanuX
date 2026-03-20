package version

import (
	"fmt"
	"runtime"
)

// Injected cleanly at compilation via -ldflags natively by the Makefile.
var (
	SemVer      = "v0.0.0-dev"
	Commit      = "unknown"
	BuildDate   = "unknown"
	Dirty       = "false"
	GoToolchain = runtime.Version()
)

// Identity constructs the deterministic build boundary map.
type Identity struct {
	SemVer      string `json:"semver" yaml:"semver"`
	Commit      string `json:"commit" yaml:"commit"`
	BuildDate   string `json:"build_date" yaml:"build_date"`
	Dirty       string `json:"dirty_tree" yaml:"dirty_tree"`
	GoToolchain string `json:"go_version" yaml:"go_version"`
}

// Get dynamically renders the compiled boundaries.
func Get() Identity {
	return Identity{
		SemVer:      SemVer,
		Commit:      Commit,
		BuildDate:   BuildDate,
		Dirty:       Dirty,
		GoToolchain: GoToolchain,
	}
}

// String provides the default terminal table format.
func (i Identity) String() string {
	return fmt.Sprintf("QuanuX CLI Engine %s\nCommit: %s (Dirty: %s)\nBuilt: %s\nOperating System: %s/%s\nCompiler: %s",
		i.SemVer, i.Commit, i.Dirty, i.BuildDate, runtime.GOOS, runtime.GOARCH, i.GoToolchain)
}
