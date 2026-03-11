#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import urllib.request
import os
import hashlib
from datetime import datetime

# Official FIX Orchestra Repository URL (Latest EP)
FIX_REPO_URL = "https://raw.githubusercontent.com/FIXTradingCommunity/orchestra/master/repository/FIX.latest.xml"
OUTPUT_DIR = "QuanuX-Orchestra/include/quanux/orchestra"
CONSTANTS_FILE = os.path.join(OUTPUT_DIR, "constants.hpp")

def download_orchestra():
    print(f"[*] Downloading FIX Orchestra from {FIX_REPO_URL}...")
    try:
        response = urllib.request.urlopen(FIX_REPO_URL)
        return response.read()
    except Exception as e:
        print(f"[!] Failed to download: {e}")
        # Try local fallback if internet is blocked
        if os.path.exists("QuanuX-Orchestra/scripts/fix_repository.xml"):
             with open("QuanuX-Orchestra/scripts/fix_repository.xml", "rb") as f:
                  return f.read()
        return None

def generate_checksum(data):
    return hashlib.sha256(data).hexdigest()

def parse_and_generate(xml_data):
    print("[*] Parsing Orchestra XML...")
    root = ET.fromstring(xml_data)
    
    # Simple namespace handling if present
    ns = {'fixr': 'http://fixprotocol.io/2020/orchestra/repository'}
    
    fields = []
    # Try finding fields with or without namespaces
    fields_xml = root.findall('.//fixr:fields/fixr:field', ns)
    if not fields_xml:
         fields_xml = root.findall('.//fields/field')

    if not fields_xml:
         print("[!] Could not parse fields. Check XML structure.")
         return None
         
    for field in fields_xml:
        tag = field.get('id')
        name = field.get('name')
        type_str = field.get('type')
        if tag and name:
            fields.append((tag, name, type_str))
            
    # Add our compliance hook
    fields.append(("99999", "QuanuxUnmappedTag", "String"))
    
    print(f"[*] Extracted {len(fields)} fields.")
    
    codesets = []
    codesets_xml = root.findall('.//fixr:codeSets/fixr:codeSet', ns)
    if not codesets_xml:
         codesets_xml = root.findall('.//codeSets/codeSet')
         
    for cs in codesets_xml:
        name = cs.get('name')
        cs_type = cs.get('type')
        codes = []
        for code in cs.findall('.//fixr:code', ns) or cs.findall('.//code'):
            cname = code.get('name')
            cvalue = code.get('value')
            if cname and cvalue:
                codes.append((cname, cvalue))
        if name and codes:
            codesets.append((name, cs_type, codes))
            
    print(f"[*] Extracted {len(codesets)} codeSets.")
    return fields, codesets

def write_hpp(fields, codesets, checksum):
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    with open(CONSTANTS_FILE, 'w') as f:
        f.write("/**\n")
        f.write(" * QuanuX-Orchestra: The Rosetta Stone\n")
        f.write(" * Auto-generated FIX Orchestra Constants\n")
        f.write(f" * SHA-256 Checksum: {checksum}\n")
        f.write(f" * Generated: {datetime.utcnow().isoformat()}Z\n")
        f.write(" */\n\n")
        f.write("#pragma once\n\n")
        f.write("#include <cstdint>\n\n")
        f.write("namespace quanux {\n")
        f.write("namespace orchestra {\n\n")
        
        f.write("enum class FixTag : uint32_t {\n")
        for tag, name, _ in fields:
            # Handle standard names that might conflict with C++ keywords
            safe_name = name.replace("-", "_").replace(" ", "")
            f.write(f"    {safe_name} = {tag},\n")
        f.write("};\n\n")
        
        for name, cs_type, codes in codesets:
            c_type = "char" if cs_type == "char" else "int"
            clean_name = name.replace("CodeSet", "")
            f.write(f"enum class {clean_name} : {c_type} {{\n")
            for cname, cval in codes:
                val_str = f"'{cval}'" if c_type == 'char' else cval
                f.write(f"    {cname} = {val_str},\n")
            f.write("};\n\n")
        
        f.write("} // namespace orchestra\n")
        f.write("} // namespace quanux\n")
    print(f"[+] Successfully generated {CONSTANTS_FILE}")

def main():
    xml_data = download_orchestra()
    if not xml_data:
        print("[!] No XML data available. Exiting.")
        return
        
    checksum = generate_checksum(xml_data)
    print(f"[*] SHA-256 Checksum: {checksum}")
    
    fields, codesets = parse_and_generate(xml_data)
    if fields:
        write_hpp(fields, codesets, checksum)

if __name__ == "__main__":
    main()
