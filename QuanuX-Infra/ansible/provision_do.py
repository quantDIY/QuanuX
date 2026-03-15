import os
import sys
import subprocess

# Add the root directory to path so we can import server.security.secrets
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..')))

from server.security.secrets import KeyringBackend

def run_terraform():
    print("Fetching securely stored DigitalOcean tokens from OS Keyring...")
    kb = KeyringBackend()
    
    # We prefix with QUANUX_ as seen in the user's terminal output: `✅ Set QUANUX_QuanuX-Omega-Droplets`
    droplets_token = kb.get("QUANUX_QuanuX-Omega-Droplets")

    # The user provided the explicit string for the access ID
    spaces_access_id = "DO00DTZWAFADHH96HV37"
    # And the secret key is within the keyring pointing to this
    spaces_secret_key = kb.get("QUANUX_Terraform-DO-QuanuX")

    if not droplets_token:
        print("[FATAL ERROR] Could not retrieve Droplets DO token from keyring (QUANUX_QuanuX-Omega-Droplets).")
        sys.exit(1)
        
    if not spaces_access_id or not spaces_secret_key:
        print("[FATAL ERROR] Could not retrieve Spaces S3 credentials from keyring.")
        sys.exit(1)

    print("Tokens successfully retrieved. Injecting into Terraform Execution Environment...")
    
    # Map the tokens into environment variables matching the Terraform variables
    env = os.environ.copy()
    env["TF_VAR_do_token_droplets"] = droplets_token
    env["TF_VAR_spaces_access_id"] = spaces_access_id
    env["TF_VAR_spaces_secret_key"] = spaces_secret_key
    
    # Fetch SSH keys dynamically from DO to ensure we can ssh into the droplets
    try:
        import requests, json
        headers = {'Authorization': f'Bearer {droplets_token}', 'Content-Type': 'application/json'}
        response = requests.get('https://api.digitalocean.com/v2/account/keys', headers=headers)
        ssh_keys = [str(k['fingerprint']) for k in response.json().get('ssh_keys', [])]
        if ssh_keys:
            env["TF_VAR_ssh_keys"] = json.dumps(ssh_keys)
        else:
            print("[WARNING] No SSH Keys found on DO account. You will be emailed root passwords.")
    except Exception as e:
        print(f"[ERROR] Failed to fetch SSH keys from DO: {e}")
    
    # Compute Admin IP for dynamic Paranoia Firewall inclusion
    try:
        my_ip = subprocess.check_output(["curl", "-s", "https://api.ipify.org"]).decode('utf8').strip()
    except Exception:
        my_ip = "127.0.0.1"
        
    if "TF_VAR_admin_ip" not in env:
        # Default the admin_ip to current IP so the firewall rule complies.
        env["TF_VAR_admin_ip"] = my_ip
        
    terraform_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'QuanuX-Infra', 'terraform'))
    
    print("Running terraform init...")
    subprocess.run(["terraform", "init"], cwd=terraform_dir, env=env, check=True)

    print("Running terraform apply...")
    subprocess.run(["terraform", "apply", "-auto-approve"], cwd=terraform_dir, env=env, check=True)
    
    print("Terraform DO Execution Complete.")

if __name__ == "__main__":
    run_terraform()
