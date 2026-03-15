import re
import sys

# The issue is because `enum class BeginString : int` explicitly sizes it to `int` (32-bit).
# Since our packed hex values for string representations like 'FIXT11' or 'CorporatePrivatePlacement'
# easily cross 64-bit bounds, we can't shove them into a 32-bit enum without narrowing errors.
# The cleanest fix is to use Python's hashlib or just a simple deterministic 32-bit hash.
# `hash()` is different per run in Python 3. We'll use FNV-1a 32-bit.

def fnv1a_32(string):
    hash_val = 0x811c9dc5
    for char in string.encode('utf-8'):
        hash_val ^= char
        hash_val = (hash_val * 0x01000193) & 0xFFFFFFFF
    
    # Treat as signed 32-bit integer string for C++ enum
    if hash_val & 0x80000000:
        hash_val = -((hash_val ^ 0xFFFFFFFF) + 1)
    
    return str(hash_val)

def fix_constants(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Need to revert the previous 0xULL script replacements back to hash values.
    # Pattern to match both `Name = Value,` and `Name = 0x...ULL,`
    # Also reset the previously mutated string.
    # It's safer to just run a regex that captures anything on the RHS and we'll apply it 
    # if the RHS is not a plain int, or if it IS a 0x ULL hex.
    
    def replacer(match):
        prefix = match.group(1)
        name = match.group(2)
        eq = match.group(3)
        val = match.group(4)
        comma = match.group(5)
        
        if val in ['true', 'false']:
            return match.group(0)
            
        # Leave integers
        if val.lstrip('-').isdigit():
            return match.group(0)
            
        try:
            float(val)
            return match.group(0)
        except ValueError:
            pass
            
        # Is it a previously patched hex literal?
        if val.startswith("0x") and val.endswith("ULL"):
            # Reconstruct the string from the hex value.
            hex_str = val[2:-3]
            try:
                # Pad to even bytes
                if len(hex_str) % 2 != 0:
                    hex_str = "0" + hex_str
                original_bytes = bytes.fromhex(hex_str)
                original_str = original_bytes.decode('utf-8')
                val = original_str
            except:
                pass # If reconstruction fails somehow, hash the hex string itself.

        hashed = fnv1a_32(val)
        return f"{prefix}{name}{eq}{hashed}{comma}"

    # Match anything after =, excluding space
    pattern = re.compile(r'(\s+)(\w+)(\s*=\s*)([A-Za-z0-9\.\-]+)(,)')
    fixed_content = pattern.sub(replacer, content)

    # Some of the C++ enums enforce char. Example: enum class AdvSide : char
    # We must reset those to actual numbers to stop narrowing bounds from 32-bit hash.
    fixed_content = re.sub(r'enum class (\w+)\s*:\s*char\s*\{', r'enum class \1 : int {', fixed_content)


    with open(filepath, 'w') as f:
        f.write(fixed_content)

if __name__ == "__main__":
    fix_constants(sys.argv[1])
