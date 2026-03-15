import re
import sys

def fix_constants(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # We are looking for enum values like:  Name = VALUE,
    # where VALUE is a string of uppercase letters, possibly with numbers or dots,
    # and it's NOT already quoted or a single integer.
    
    # We will use a regex that looks for:
    # 1. Start of line whitespace
    # 2. Variable name (alphanumeric)
    # 3. Space = Space
    # 4. the bad value (letters, dots, digits but MUST start with a letter)
    # 5. comma
    
    def replacer(match):
        prefix = match.group(1)
        name = match.group(2)
        eq = match.group(3)
        val = match.group(4)
        comma = match.group(5)
        
        # If the value is a known keyword or already quoted, skip
        if val in ['true', 'false'] or val.startswith("'") or val.startswith('"'):
            return match.group(0)
            
        # If the value is purely numeric, skip
        if val.isdigit():
            return match.group(0)
            
        # If the value is a float-like number, skip
        try:
            float(val)
            return match.group(0)
        except ValueError:
            pass
            
        # Otherwise, we need to quote it as a character or string.
        # However, C++ enums can only be integers or chars.
        # But this legacy file seems to just want these to be integers under the hood,
        # or maybe char constants. 
        # Multi-char constants emit warnings in C++ ('AB') but they do compile if treated as ints.
        # Actually, let's just quote them as single-quoted characters natively. 
        # The clang error previously mentioned "multi-character character constant [-Wmultichar]" which is just a warning.
        
        return f"{prefix}{name}{eq}'{val}'{comma}"

    # Regex: (\s+)(\w+)(\s*=\s*)([A-Za-z][A-Za-z0-9\.]*)(,)
    pattern = re.compile(r'(\s+)(\w+)(\s*=\s*)([A-Za-z][A-Za-z0-9\.]*)(,)')
    fixed_content = pattern.sub(replacer, content)

    with open(filepath, 'w') as f:
        f.write(fixed_content)

if __name__ == "__main__":
    fix_constants(sys.argv[1])
