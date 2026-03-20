import os
import re

for root, _, files in os.walk('pkg'):
    for f in files:
        if f.endswith('.go'):
            p = os.path.join(root, f)
            with open(p, 'r') as file:
                content = file.read()
            
            # Skip if output already imported
            if "github.com/QuanuX/qxctl/internal/output" not in content and ("fmt.Println(" in content or "fmt.Printf(" in content):
                content = content.replace('"fmt"', '\"fmt\"\n\t"github.com/QuanuX/qxctl/internal/output"')
            
            content = content.replace("fmt.Println(", "output.FromContext(ctx).EmitRaw(")
            content = content.replace("fmt.Printf(", "output.FromContext(ctx).EmitRawf(")
            
            with open(p, 'w') as file:
                file.write(content)

print("Replacement complete.")
