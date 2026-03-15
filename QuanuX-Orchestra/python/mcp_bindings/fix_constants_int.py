import re
import sys

# In C++, a multi-char constant is basically just evaluating to an int (e.g. 'AB' -> 0x4142).
# We can just write a quick hash or just pack the chars into uint32_t.
# The previous script caused warnings.
# Let's replace 'STRING' with a packed uint64_t representation:
# A -> 0x41
# AB -> 0x4142

def pack_string_to_int(s):
    # Pack up to 8 chars into a uint64_t literal
    s = s.replace("-", "") # some had hyphens like "kW-a"
    b = s.encode('utf-8')
    val = 0
    for byte in b:
        val = (val << 8) | byte
    return f"0x{val:X}ULL"

def fix_constants(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Pattern finding: Name = 'VALUE',
    # Since the previous script already quoted most of them as 'VALUE', we can find those.
    # We also need to find unquoted ones if any are left like kW-a
    
    # Let's just strip out all the single quotes that were added first to reset.
    content = re.sub(r"= '([^']+)',", r"= \1,", content)

    def replacer(match):
        prefix = match.group(1)
        name = match.group(2)
        eq = match.group(3)
        val = match.group(4)
        comma = match.group(5)
        
        if val in ['true', 'false']:
            return match.group(0)
            
        if val.isdigit():
            return match.group(0)
            
        try:
            float(val)
            return match.group(0)
        except ValueError:
            pass
            
        # Ignore things like `0x1A` or bit shifts if they existed (they probably don't)
        if val.startswith("0x"):
            return match.group(0)
            
        packed = pack_string_to_int(val)
        return f"{prefix}{name}{eq}{packed}{comma}"

    # Match things like `    Name = Value,` and `    Name = Value-With-Hyphens,`
    # It must start with a letter.
    pattern = re.compile(r'(\s+)(\w+)(\s*=\s*)([A-Za-z][A-Za-z0-9\.\-]*)(,)')
    fixed_content = pattern.sub(replacer, content)

    with open(filepath, 'w') as f:
        f.write(fixed_content)

if __name__ == "__main__":
    fix_constants(sys.argv[1])
