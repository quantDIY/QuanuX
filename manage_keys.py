import keyring
import argparse
import sys

SERVICE_NAME = "QuanuX"
KEY_NAME = "DATABENTO_API_KEY"

def set_key(api_key):
    try:
        keyring.set_password(SERVICE_NAME, KEY_NAME, api_key)
        print(f"Successfully stored {KEY_NAME} in keyring for service '{SERVICE_NAME}'.")
    except Exception as e:
        print(f"Failed to store key: {e}")
        sys.exit(1)

def get_key():
    try:
        key = keyring.get_password(SERVICE_NAME, KEY_NAME)
        if key:
            print(f"Found {KEY_NAME}: {key[:4]}... (redacted)")
        else:
            print(f"{KEY_NAME} not found in keyring.")
    except Exception as e:
        print(f"Failed to retrieve key: {e}")
        sys.exit(1)

def delete_key():
    try:
        keyring.delete_password(SERVICE_NAME, KEY_NAME)
        print(f"Successfully deleted {KEY_NAME} from keyring.")
    except keyring.errors.PasswordDeleteError:
         print(f"{KEY_NAME} not found in keyring to delete.")
    except Exception as e:
        print(f"Failed to delete key: {e}")
        sys.exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Manage Databento API Key via OS Keyring")
    subparsers = parser.add_subparsers(dest="command", required=True)

    set_parser = subparsers.add_parser("set", help="Set the API key")
    set_parser.add_argument("key", help="The Databento API Key")

    get_parser = subparsers.add_parser("get", help="Check if the API key is set")
    
    del_parser = subparsers.add_parser("delete", help="Delete the API key")

    args = parser.parse_args()

    if args.command == "set":
        set_key(args.key)
    elif args.command == "get":
        get_key()
    elif args.command == "delete":
        delete_key()
