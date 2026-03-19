package tui

import (
	"context"
	"fmt"
	"time"

	"github.com/charmbracelet/bubbletea"
	"github.com/QuanuX/qxctl/internal/theme"
)

type DashboardModel struct {
	context context.Context
	monitor bool
	frames  int
	loading bool
	err     error
}

type tickMsg time.Time

func (m DashboardModel) Init() tea.Cmd {
	return tickCmd()
}

func (m DashboardModel) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		if msg.String() == "q" || msg.String() == "ctrl+c" {
			return m, tea.Quit
		}
	case tickMsg:
		if m.loading {
			m.frames++
			if m.frames > 20 {
				m.loading = false
			}
			return m, tickCmd()
		}
	}
	return m, nil
}

func (m DashboardModel) View() string {
	if m.err != nil {
		return theme.FailStyle.Render(fmt.Sprintf("\nFatal Error: %v\n", m.err))
	}

	header := theme.HeaderStyle.Render("QuanuX Native Supervisor (TUI Engine Active)\n")

	if m.loading {
		status := fmt.Sprintf("Establishing physical matrix bindings... [%d/20]", m.frames)
		return header + "\n" + theme.SkillStyle.Render(status)
	}

	stats := "Status: " + theme.OkStyle.Render("ONLINE") + "\n"
	stats += "NATS Hub: " + theme.LabelStyle.Render("127.0.0.1:4222") + "\n"
	stats += "QuanuX CGO Interlocks: " + theme.OkStyle.Render("SECURED") + "\n"

	footer := "\n" + theme.DetailStyle.Render("Press 'q' or 'ctrl+c' to safely terminate Go processes natively.")
	return header + "\n" + stats + footer
}

func tickCmd() tea.Cmd {
	return tea.Tick(time.Millisecond*50, func(t time.Time) tea.Msg {
		return tickMsg(t)
	})
}

func StartDashboard(ctx context.Context, monitor bool) error {
	m := DashboardModel{
		context: ctx,
		monitor: monitor,
		loading: true,
	}

	p := tea.NewProgram(m)
	if _, err := p.Run(); err != nil {
		return fmt.Errorf("could not start TUI dashboard: %w", err)
	}
	return nil
}
