#!/usr/bin/env python3
import json
import subprocess
import os

def get_terraform_outputs():
    # Run terraform output -json to extract the active IPs securely
    terraform_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../terraform"))
    try:
        result = subprocess.run(["terraform", "output", "-json"], capture_output=True, text=True, cwd=terraform_dir, check=True)
        return json.loads(result.stdout)
    except subprocess.CalledProcessError:
        return {}
    except AttributeError:
        # Happens if terraform hasn't been applied yet
        return {}

def build_inventory():
    outputs = get_terraform_outputs()
    
    inventory = {
        "_meta": {
            "hostvars": {}
        },
        "panopticon_ledger": {
            "hosts": []
        },
        "panopticon_buffer": {
            "hosts": []
        },
        "panopticon_forge": {
            "hosts": []
        },
        "panopticon_vault": {
            "hosts": []
        },
        "panopticon_oracle": {
            "hosts": []
        },
        "panopticon_nexus": {
            "hosts": []
        },
        "edge_nodes": {
            "hosts": []
        }
    }

    if not outputs:
        return inventory

    # Panopticon Ledger Node mapping (OpenSearch)
    if "quanux_panopticon_ledger_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_ledger_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_ledger_internal_ip"]["value"]
        inventory["panopticon_ledger"]["hosts"].append("panopticon-ledger")
        inventory["_meta"]["hostvars"]["panopticon-ledger"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Panopticon Buffer Node mapping (ValKey & Python)
    if "quanux_panopticon_buffer_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_buffer_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_buffer_internal_ip"]["value"]
        inventory["panopticon_buffer"]["hosts"].append("panopticon-buffer")
        inventory["_meta"]["hostvars"]["panopticon-buffer"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Panopticon Forge Node
    if "quanux_panopticon_forge_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_forge_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_forge_internal_ip"]["value"]
        inventory["panopticon_forge"]["hosts"].append("panopticon-forge")
        inventory["_meta"]["hostvars"]["panopticon-forge"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Panopticon Vault Node
    if "quanux_panopticon_vault_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_vault_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_vault_internal_ip"]["value"]
        inventory["panopticon_vault"]["hosts"].append("panopticon-vault")
        inventory["_meta"]["hostvars"]["panopticon-vault"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Panopticon Oracle Node
    if "quanux_panopticon_oracle_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_oracle_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_oracle_internal_ip"]["value"]
        inventory["panopticon_oracle"]["hosts"].append("panopticon-oracle")
        inventory["_meta"]["hostvars"]["panopticon-oracle"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Panopticon Nexus Node
    if "quanux_panopticon_nexus_public_ip" in outputs:
        pub_ip = outputs["quanux_panopticon_nexus_public_ip"]["value"]
        priv_ip = outputs["quanux_panopticon_nexus_internal_ip"]["value"]
        inventory["panopticon_nexus"]["hosts"].append("panopticon-nexus")
        inventory["_meta"]["hostvars"]["panopticon-nexus"] = {
            "ansible_host": pub_ip,
            "ansible_user": "root",
            "internal_ip": priv_ip
        }

    # Edge Nodes mapping
    if "quanux_edge_nyc_public_ip" in outputs:
        pub_ip1 = outputs["quanux_edge_nyc_public_ip"]["value"]
        priv_ip1 = outputs["quanux_edge_nyc_internal_ip"]["value"]
        inventory["edge_nodes"]["hosts"].append("edge-nyc-1")
        inventory["_meta"]["hostvars"]["edge-nyc-1"] = {
            "ansible_host": pub_ip1,
            "ansible_user": "root",
            "internal_ip": priv_ip1
        }
        
    if "quanux_edge_nyc_2_public_ip" in outputs:
        pub_ip2 = outputs["quanux_edge_nyc_2_public_ip"]["value"]
        priv_ip2 = outputs["quanux_edge_nyc_2_internal_ip"]["value"]
        inventory["edge_nodes"]["hosts"].append("edge-nyc-2")
        inventory["_meta"]["hostvars"]["edge-nyc-2"] = {
            "ansible_host": pub_ip2,
            "ansible_user": "root",
            "internal_ip": priv_ip2
        }

    return inventory

if __name__ == "__main__":
    print(json.dumps(build_inventory(), indent=2))
