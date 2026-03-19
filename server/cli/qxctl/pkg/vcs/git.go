package vcs

import (
	"context"
	"fmt"
	"os"

	"github.com/go-git/go-git/v5"
	"github.com/QuanuX/qxctl/internal/theme"
)

// Status reads the Git directory natively through CGO file handles instead of `subprocess.run()`.
func Status(ctx context.Context) error {
	dir, _ := os.Getwd()
	fmt.Println(theme.HeaderStyle.Render(fmt.Sprintf("QuanuX Native VCS Engine (Path: %s)", dir)))

	repo, err := git.PlainOpenWithOptions(dir, &git.PlainOpenOptions{DetectDotGit: true})
	if err != nil {
		fmt.Println(theme.FailStyle.Render("[X] Not a git repository natively recognized by go-git."))
		return err
	}

	worktree, err := repo.Worktree()
	if err != nil {
		return err
	}

	status, err := worktree.Status()
	if err != nil {
		return err
	}

	if status.IsClean() {
		fmt.Println(theme.OkStyle.Render("[✔] Working directory completely clean natively. Zero uncommitted frames."))
	} else {
		fmt.Println(theme.SkillStyle.Render(status.String()))
	}
	return nil
}

// Commit directly edits the root .git/objects graph directly using `go-git` without invoking the OS binaries.
func Commit(ctx context.Context, message string, all bool) error {
	dir, _ := os.Getwd()
	
	repo, err := git.PlainOpenWithOptions(dir, &git.PlainOpenOptions{DetectDotGit: true})
	if err != nil {
		return err
	}

	worktree, err := repo.Worktree()
	if err != nil {
		return err
	}

	if all {
		fmt.Println(theme.DetailStyle.Render("Staging all tracked modifications natively..."))
		err = worktree.AddWithOptions(&git.AddOptions{All: true})
		if err != nil {
			return err
		}
	}

	if message == "" {
		message = "auto-commit(vcs): Native Go-Git Execution Frame"
	}

	commit, err := worktree.Commit(message, &git.CommitOptions{})
	if err != nil {
		return fmt.Errorf("native commit failure: %w", err)
	}

	fmt.Println(theme.OkStyle.Render(fmt.Sprintf("[✔] Native Commit Secured: %s", commit.String()[:8])))
	return nil
}
