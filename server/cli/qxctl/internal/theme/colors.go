package theme

import "github.com/charmbracelet/lipgloss"

var (
	// QuanuX Figma Design System Map
	Cyan    = lipgloss.Color("#00FFFF") 
	Green   = lipgloss.Color("#00FF00") 
	Red     = lipgloss.Color("#FF0000") 
	Grey    = lipgloss.Color("#666666") 
	Magenta = lipgloss.Color("#FF00FF") 

	// Standardized Typography Boundaries
	HeaderStyle  = lipgloss.NewStyle().Bold(true).Foreground(Cyan).MarginBottom(1)
	OkStyle      = lipgloss.NewStyle().Bold(true).Foreground(Green)
	FailStyle    = lipgloss.NewStyle().Bold(true).Foreground(Red)
	LabelStyle   = lipgloss.NewStyle().Width(35)
	DetailStyle  = lipgloss.NewStyle().Italic(true).Foreground(Grey).MarginLeft(4)
	SkillStyle   = lipgloss.NewStyle().Bold(true).Foreground(Magenta).MarginBottom(1)
)
