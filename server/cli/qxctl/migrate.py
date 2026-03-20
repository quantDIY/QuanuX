import glob
import re
import os

files = glob.glob("cmd/*.go")

for file in files:
    if file == "cmd/root.go":
        continue
    
    with open(file, 'r') as f:
        content = f.read()

    # Step 1: Add internal/runtime import if missing
    if '"github.com/QuanuX/qxctl/internal/runtime"' not in content:
        content = content.replace('"github.com/spf13/cobra"', '"github.com/QuanuX/qxctl/internal/runtime"\n\t"github.com/spf13/cobra"')
    
    # Step 2: Strip init() function blocks entirely
    content = re.sub(r'func init\(\)\s*\{[^\}]+\}', '', content)
    
    # Step 3: Find root vars like `var [x]Cmd = &cobra.Command{`
    # We want to keep subcommands wrapped in the same NewCmd() function for now,
    # or just wrap the top-level command.
    
    # Find the top-level `var XCmd = &cobra.Command`
    # Extract the name to capitalize it e.g. `probeCmd` -> `Probe`
    matches = re.finditer(r'var\s+([a-zA-Z0-9_]+Cmd)\s*=\s*\&cobra\.Command\{', content)
    
    for match in matches:
        var_name = match.group(1) # e.g. probeCmd
        func_name = var_name[0].upper() + var_name[1:] # e.g. ProbeCmd
        
        # We replace `var probeCmd =` with `func NewProbeCmd(app *runtime.App) *cobra.Command { \n cmd := `
        # but wait, it's easier to just do:
        content = content.replace(
            f"var {var_name} = &cobra.Command{{",
            f"func New{func_name}(app *runtime.App) *cobra.Command {{\n\tcmd := &cobra.Command{{"
        )
        
        # We need to add `return cmd \n}` at the end of the struct declaration.
        # This is tricky with raw regex. Let's do a naive string replacement of the matching `},`
        # Or better: simply replace `rootCmd.AddCommand` since we wiped init().
        
    with open(file, 'w') as f:
        f.write(content)

print("Migration script executed.")
