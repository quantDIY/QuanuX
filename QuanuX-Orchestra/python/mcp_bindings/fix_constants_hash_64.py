import re
import sys

def fnv1a_64(string):
    hash_val = 0xcbf29ce484222325
    fnv_prime = 0x100000001b3
    for char in string.encode('utf-8'):
        hash_val ^= char
        hash_val = (hash_val * fnv_prime) & 0xFFFFFFFFFFFFFFFF
    return f"{hash_val}ULL"

def fix_constants(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    generated_hashes = {}
    collision_detected = False
    
    def replacer(match):
        nonlocal collision_detected
        prefix = match.group(1)
        name = match.group(2)
        eq = match.group(3)
        val = match.group(4)
        comma = match.group(5)
        
        # Keep numeric values
        if val.lstrip('-').isdigit() or val in ['true', 'false'] or val.startswith("0x"):
            return match.group(0)
            
        try:
            float(val)
            return match.group(0)
        except ValueError:
            pass
            
        # If it's pure chars or symbols, calculate hash
        hashed = fnv1a_64(val)
        
        # Check collision
        if hashed in generated_hashes and generated_hashes[hashed] != val:
            print(f"[FATAL ERROR] Hash collision detected between '{generated_hashes[hashed]}' and '{val}' -> {hashed}")
            collision_detected = True
        generated_hashes[hashed] = val
        
        return f"{prefix}{name}{eq}{hashed}{comma}"

    # Target assignment matches
    pattern = re.compile(r'(\s+)(\w+)(\s*=\s*)([A-Za-z0-9\.\-\?\_]+)(,)')
    fixed_content = pattern.sub(replacer, content)

    # Change enum class types to use uint64_t to prevent narrowing errors
    fixed_content = re.sub(r'enum class (\w+)\s*:\s*(int|char)\s*\{', r'enum class \1 : uint64_t {', fixed_content)

    if collision_detected:
        print("[FATAL ERROR] Zero-collision proof FAILED. Aborting write.")
        sys.exit(1)

    print(f"[SUCCESS] Zero-collision proof PASSED. Tracked {len(generated_hashes)} unique 64-bit FNV-1a hashes.")

    with open(filepath, 'w') as f:
        f.write(fixed_content)

if __name__ == "__main__":
    fix_constants(sys.argv[1])
