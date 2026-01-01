#!/usr/bin/env python3
"""
QuanuX Version Management Tool
Reads meta/version_config.json and updates versions across the codebase.

Usage:
    python tools/manage_version.py check
    python tools/manage_version.py set 0.0.2
    python tools/manage_version.py bump patch|minor|major
"""

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

# Constants
ROOT_DIR = Path(__file__).parent.parent.resolve()
CONFIG_PATH = ROOT_DIR / "meta" / "version_config.json"

class VersionManager:
    def __init__(self, config_path: Path):
        self.config_path = config_path
        with open(config_path, "r") as f:
            self.config = json.load(f)
        self.targets = self.config["targets"]
        self.source_def = self.config.get("source_of_truth")

    def get_current_version(self) -> str:
        """Reads the current version from the Source of Truth."""
        if not self.source_def:
            raise ValueError("No source_of_truth defined in config.")
        
        file_path = ROOT_DIR / self.source_def["file"]
        if not file_path.exists():
            raise FileNotFoundError(f"Source file not found: {file_path}")

        pattern = self.source_def["regex"]
        content = file_path.read_text(encoding="utf-8")
        match = re.search(pattern, content)
        if not match:
            raise ValueError(f"Could not find version in source file: {file_path} with pattern: {pattern}")
        
        return match.group("version")

    def parse_version(self, version: str) -> List[int]:
        return [int(x) for x in version.split(".")]

    def format_version(self, parts: List[int]) -> str:
        return ".".join(str(p) for p in parts)

    def bump_version(self, current_version: str, bump_type: str) -> str:
        parts = self.parse_version(current_version)
        if len(parts) != 3:
            raise ValueError(f"Version must be major.minor.patch, got: {current_version}")
        
        if bump_type == "major":
            parts[0] += 1
            parts[1] = 0
            parts[2] = 0
        elif bump_type == "minor":
            parts[1] += 1
            parts[2] = 0
        elif bump_type == "patch":
            parts[2] += 1
        else:
            raise ValueError(f"Invalid bump type: {bump_type}")
        
        return self.format_version(parts)

    def update_file(self, target: Dict[str, Any], new_version: str, dry_run: bool = False):
        file_path = ROOT_DIR / target["file"]
        if not file_path.exists():
            print(f"⚠️  Skipping missing file: {target['file']}")
            return

        print(f"{'[DRY RUN] ' if dry_run else ''}Updating {target['file']} -> {new_version}")
        
        if dry_run:
            return

        content = file_path.read_text(encoding="utf-8")
        updated = False

        if target["type"] == "regex":
            pattern = target["regex"]
            # We want to replace only the captured group "version"
            # But re.sub doesn't support replacing just a group easily without lambda
            # So we assume the regex matches the whole string including the version
            # And we need to reconstruct it? 
            # Actually, standard approach: pattern should match the wrapper, 
            # and we substitute the whole match, or we use a lambda to replace group.
            
            # Let's assume the regex contains a named group (?P<version>...)
            # We will substitute the occurrences.
            
            def repl(m):
                # Reconstruct the string replacing the named group
                # This is tricky generically.
                # Simpler: The regex MUST have a group named 'version'. 
                # We replace the content of that group.
                
                # Check if 'version' group exists
                if not m.groupdict().get("version"):
                     return m.group(0)
                
                start_idx = m.start("version")
                end_idx = m.end("version")
                
                # Relative to the match
                match_start = m.start()
                
                # We can't easily edit the match object.
                # Strategy: Use the full match, replace the matched version string within it.
                # BUT what if the version string appears elsewhere in the match?
                # Best approach: string slicing on the original text? No, re.sub passes the match.
                
                # Let's perform a simple replace of the value captured by the group
                # logic: entire_match.replace(old_version, new_version)
                # Risk: if old_version appears twice.
                
                # Robust Logic:
                full_match = m.group(0)
                v_span = m.span("version")
                m_span = m.span(0)
                
                # relative indices in full_match
                rel_start = v_span[0] - m_span[0]
                rel_end = v_span[1] - m_span[0]
                
                return full_match[:rel_start] + new_version + full_match[rel_end:]

            new_content = re.sub(pattern, repl, content)
            if new_content != content:
                file_path.write_text(new_content, encoding="utf-8")
                updated = True

        elif target["type"] == "json":
            data = json.loads(content)
            keys = target["path"].split(".")
            ref = data
            for key in keys[:-1]:
                ref = ref.get(key, {})
            
            if ref and keys[-1] in ref:
                if ref[keys[-1]] != new_version:
                    ref[keys[-1]] = new_version
                    # Write back json with indentation
                    # Note: Try to preserve formatting? 
                    # standard json.dump might reformat the whole file.
                    # Ideally we use a round-trip parser, but for now standard json is explicitly chosen.
                    file_path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")
                    updated = True
        
        elif target["type"] == "toml":
            # Basic textual replacement for TOML to preserve comments/structure 
            # if we don't have a reliable round-trip TOML library installed (like tomlkit).
            # The config specifies "toml" but for now, we'll try to fallback to a regex if possible OR
            # check if tomlkit is available. In this environment, we may not have it.
            # Fallback: Treat as regex if a regex is provided, or try rough string match for key.
            
            # Given we specified 'path' in config, we imply semantic understanding.
            # But let's be pragmatic.
            # server/cli/pyproject.toml -> version = "..."
            # src-tauri/Cargo.toml -> version = "..."
            
            # We will do a line-by-line scan for `key = "value"` pattern matching the path last component.
            lines = content.splitlines()
            key_name = target["path"].split(".")[-1]
            new_lines = []
            file_updated = False
            
            section_stack = [] # Rough section tracking if needed, but usually unique keys exist.
            
            for line in lines:
                # Naive matching: version = "0.0.1" 
                # We need to be careful about which section.
                # "package.version" -> [package] section.
                
                stripped = line.strip()
                if stripped.startswith("[") and stripped.endswith("]"):
                    # Section header (rough)
                    pass
                    
                # Regex for key = "value"
                # Matches: key = "old_val"
                # We want to replace "old_val" with new_version
                
                # This naive approach is risky if keys are duplicated.
                # But for 'version', it's usually top-level or in [package].
                
                if re.match(f'^{key_name}\\s*=\\s*"[^"]+"', stripped):
                    # Found it
                    new_line = re.sub(f'{key_name}\\s*=\\s*"[^"]+"', f'{key_name} = "{new_version}"', line)
                    if new_line != line:
                        new_lines.append(new_line)
                        file_updated = True
                        continue
                
                new_lines.append(line)
            
            if file_updated:
                 file_path.write_text("\n".join(new_lines) + "\n", encoding="utf-8")
                 updated = True

        if updated:
            print(f"✅ Updated {target['file']}")
        else:
            print(f"➖ No change needed for {target['file']}")

    def run(self, args):
        current_v = self.get_current_version()
        print(f"Current Version: {current_v}")

        if args.command == "check":
            # Just listing current matches
            return

        new_v = current_v
        if args.command == "set":
            new_v = args.version
        elif args.command == "bump":
            new_v = self.bump_version(current_v, args.type)

        print(f"Target Version:  {new_v}")
        if args.dry_run:
            print("--- DRY RUN MODE ---")

        for target in self.targets:
            self.update_file(target, new_v, dry_run=args.dry_run)

def main():
    parser = argparse.ArgumentParser(description="Manage QuanuX versions.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Check
    subparsers.add_parser("check", help="Check current version configuration")

    # Set
    p_set = subparsers.add_parser("set", help="Set to specific version")
    p_set.add_argument("version", help="Version string (e.g. 0.0.2)")
    p_set.add_argument("--dry-run", action="store_true")

    # Bump
    p_bump = subparsers.add_parser("bump", help="Bump version part")
    p_bump.add_argument("type", choices=["major", "minor", "patch"])
    p_bump.add_argument("--dry-run", action="store_true")

    args = parser.parse_args()

    try:
        vm = VersionManager(CONFIG_PATH)
        vm.run(args)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
