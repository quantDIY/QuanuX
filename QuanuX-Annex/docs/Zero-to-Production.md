# QuanuX-Annex Zero-to-Production Guide

The definitive QuanuX-Annex deployment sequence. This guide will take a completely bare machine and instantly deploy a fully-managed DigitalOcean Edge Drop with zero manual configuration. 

## PREREQUISITES

1. You must have a DigitalOcean API Token (Read/Write).
2. You must have a DigitalOcean Spaces Access ID and Secret.
3. Your local machine must have `quanuxctl` installed.

## 1. SECURING CREDENTIALS TO OS KEYRING

The core principle of QuanuX is that secrets are **never** stored in plain-text files or committed to Git. All credentials must be injected directly into your native OS Keychain using `quanuxctl`.

1. **Inject the DigitalOcean Infrastructure Token**:
   ```bash
   quanuxctl infra set-token <YOUR_DO_TOKEN>
   ```

2. **Inject the DigitalOcean Spaces Credentials**:
   ```bash
   quanuxctl infra set-spaces-keys <YOUR_ACCESS_KEY_ID> <YOUR_SECRET_KEY>
   ```

## 2. IGNITING THE TERRAFORM CLOUD MATRIX

With secrets securely locked away, the Terraform execution extracts credentials purely from the system runtime envelope.

1. Navigate to the Infrastructure Directory:
   ```bash
   cd QuanuX-Infra/terraform
   ```
2. Spawn the ephemeral authorization shell and execute Terraform:
   ```bash
   eval $(quanuxctl infra auth-shell) && terraform init && terraform apply -auto-approve
   ```

This spins up the `$5/mo` NATS/JetStream instances and the memory-aligned `c-2` C++ droplets automatically.

## 3. ZERO-ALLOCATION ANSIBLE DROP

Once the OS hardware is spun up, we drop the exact compiled payloads.

1. Navigate to the Ansible Directory:
   ```bash
   cd QuanuX-Infra/ansible
   ```
2. Disable Host Key Checking to prevent blockages on fresh Droplet IPs and execute the payload drop:
   ```bash
   export ANSIBLE_HOST_KEY_CHECKING=False && ansible-playbook -i dynamic_inventory.py 03e-annex-core-drop.yml
   ```

This step rigorously installs C++ libraries natively, applies kernel tuning, compiles QuanuX-Annex out of standard strings directly into memory buffers, and mounts the asynchronous Streaming Server via systemd natively.

## 4. VALIDATING QUANUX INFRASTRUCTURE

Verification is integrated natively deeply within `quanuxctl`. Execute the following commands right from your command line to list your spun-up DigitalOcean components and ensure availability!

```bash
# Retrieve provisioned droplet IP addresses:
quanuxctl infra do-droplets

# Retrieve your dynamically generated S3 API endpoints for Hasura:
quanuxctl infra do-spaces
```

You are now operating at a Tier 1 High-Frequency Data Retrieval standard. Welcome to QuanuX.
