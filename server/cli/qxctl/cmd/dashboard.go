package cmd

import (
	"fmt"
	"os"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var dashboardCmd = &cobra.Command{
	Use:   "dashboard",
	Short: "Launch System Dashboard (TUI)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("dashboard invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var titleStyle = lipgloss.NewStyle().
	Foreground(lipgloss.Color("#00FF00")).
	Bold(true).
	Padding(1, 2)

type model struct {
	quitting bool
	url      string
}

func initialModel() model {
	return model{url: viper.GetString("dashboard.dashboard.start.url")}
}

func (m model) Init() tea.Cmd {
	return nil
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "q", "ctrl+c", "esc":
			m.quitting = true
			return m, tea.Quit
		}
	}
	return m, nil
}

func (m model) View() string {
	if m.quitting {
		return "Exiting Dashboard...\n"
	}
	return titleStyle.Render("QuanuX Terminal Dashboard") +
		fmt.Sprintf("\n\nConnecting to NATS Hub: %s\n", m.url) +
		"\nPress 'q' or ESC to quit.\n(Mock telemetry streams plotting...)"
}

var dashboardStartCmd = &cobra.Command{
	Use:   "start",
	Short: "Launch the QuanuX System Dashboard (TUI)",
	Run: func(cmd *cobra.Command, args []string) {
		p := tea.NewProgram(initialModel(), tea.WithAltScreen())
		if _, err := p.Run(); err != nil {
			fmt.Printf("Alas, there's been an error: %v", err)
			os.Exit(1)
		}
	},
}

func init() {
	rootCmd.AddCommand(dashboardCmd)
	dashboardCmd.AddCommand(dashboardStartCmd)
	dashboardStartCmd.Flags().String("url", "nats://localhost:4222", "NATS Hub URL")
	viper.BindPFlag("dashboard.dashboard.start.url", dashboardStartCmd.Flags().Lookup("url"))
}

