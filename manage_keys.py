import keyring
import argparse
import sys

SERVICE_NAME = "QuanuX"
DEFAULT_KEY_NAME = "DATABENTO_API_KEY"

def set_key(key_name, api_key):
    try:
        keyring.set_password(SERVICE_NAME, key_name, api_key)
        print(f"Successfully stored {key_name} in keyring for service '{SERVICE_NAME}'.")
    except Exception as e:
        print(f"Failed to store key: {e}")
        sys.exit(1)

def get_key(key_name):
    try:
        key = keyring.get_password(SERVICE_NAME, key_name)
        if key:
            print(f"Found {key_name}: {key[:4]}... (redacted)")
        else:
            print(f"{key_name} not found in keyring.")
    except Exception as e:
        print(f"Failed to retrieve key: {e}")
        sys.exit(1)

def delete_key(key_name):
    try:
        keyring.delete_password(SERVICE_NAME, key_name)
        print(f"Successfully deleted {key_name} from keyring.")
    except keyring.errors.PasswordDeleteError:
         print(f"{key_name} not found in keyring to delete.")
    except Exception as e:
        print(f"Failed to delete key: {e}")
        sys.exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Manage API Keys via OS Keyring")
    parser.add_argument("--name", default=DEFAULT_KEY_NAME, help=f"Key name (default: {DEFAULT_KEY_NAME})")
    
    subparsers = parser.add_subparsers(dest="command", required=True)

    set_parser = subparsers.add_parser("set", help="Set the API key")
    set_parser.add_argument("key", help="The API Key value")

    get_parser = subparsers.add_parser("get", help="Check if the API key is set")
    
    del_parser = subparsers.add_parser("delete", help="Delete the API key")

    args = parser.parse_args()

    if args.command == "set":
        set_key(args.name, args.key)
    elif args.command == "get":
        get_key(args.name)
    elif args.command == "delete":
        delete_key(args.name)
