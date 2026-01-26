#!/usr/bin/env python3
import keyring
import argparse
import getpass
import sys

SERVICE_NAME = "QuanuX_Rithmic"

def set_credentials(user, password=None):
    if not password:
        password = getpass.getpass(f"Enter password for Rithmic user '{user}': ")
    
    try:
        keyring.set_password(SERVICE_NAME, user, password)
        print(f"Successfully stored credentials for '{user}' in service '{SERVICE_NAME}'.")
    except Exception as e:
        print(f"Error storing credentials: {e}")
        sys.exit(1)

def get_credentials(user):
    try:
        password = keyring.get_password(SERVICE_NAME, user)
        if password:
            print(f"Password found for '{user}'.")
            return password
        else:
            print(f"No password found for '{user}'.")
            return None
    except Exception as e:
        print(f"Error retrieving credentials: {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Manage QuanuX Rithmic Credentials")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Set command
    set_parser = subparsers.add_parser("set", help="Store credentials")
    set_parser.add_argument("user", help="Rithmic username")
    set_parser.add_argument("--password", "-p", help="Rithmic password (optional, will prompt if omitted)")

    # Get command
    get_parser = subparsers.add_parser("get", help="Retrieve credentials (test)")
    get_parser.add_argument("user", help="Rithmic username")

    args = parser.parse_args()

    if args.command == "set":
        set_credentials(args.user, args.password)
    elif args.command == "get":
        get_credentials(args.user)

if __name__ == "__main__":
    main()
