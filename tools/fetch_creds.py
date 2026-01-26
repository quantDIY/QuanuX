import keyring
import sys

SERVICE = "QuanuX"
KEYS = ["RITHMIC_USER", "RITHMIC_PASSWORD", "RITHMIC_EXCHANGE"]

def main():
    creds = {}
    missing = []
    
    for k in KEYS:
        val = keyring.get_password(SERVICE, k)
        if val:
            creds[k] = val
        else:
            missing.append(k)
            
    if missing:
        print(f"MISSING: {', '.join(missing)}")
        if len(missing) == len(KEYS):
             sys.exit(1)
    
    # Print export format
    for k, v in creds.items():
        print(f"export {k}='{v}'")

if __name__ == "__main__":
    main()
