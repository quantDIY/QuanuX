# QuanuX Infrastructure: Internal Sprint Lessons

**Status:** Uncommitted Intelligence Extraction
**Context:** Operation Ansible Genesis Phase 4

## 1. Remote Node Setup (Agentless)
The automation successfully bootstrapped remote bare-metal DigitalOcean droplets entirely without manual user intervention. 
- **Mechanism:** Utilizing the underlying SSH keys natively provided during the Terraform droplet creation phase (`digitalocean_droplet.ssh_keys`). 
- **Execution:** Ansible's `ansible.cfg` was configured to use SSH pipelining, directly riding the authenticated connections established by the local OS SSH agent, bypassing any need for interactive password prompts or manual `authorized_keys` seeding.

## 2. Dynamic VPC IP Extraction and Injection
Hardcoding the Panopticon's internal IP (`10.10.10.x`) into edge node playbooks proved brittle when Terraform dynamically assigned addresses.
- **Mechanism:** We utilized Ansible's powerful dynamic templating across the inventory context.
- **Execution:** By invoking `{{ hostvars['panopticon-node']['internal_ip'] }}` directly within the Edge Node playbook (`03-sovereign-engine.yml`), Ansible pulled the correct, live Terraform-provisioned VPC interface IP at runtime. This dynamically bound `quanux-engine.service` directly to the NATS JetStream container without hardcoded parameters.

## 3. SSH Pipelining and NATS JetStream Binding
To intercept the closed telemetry firehose on a port actively blocked by the Paranoia Firewall, a secure bypass was required.
- **Mechanism:** Establishing an active SSH tunnel forwarding local ports directly to the remote internal VPC binding.
- **Execution:** Running `ssh -o StrictHostKeyChecking=no -L 4223:10.10.10.8:4222 root@143.198.29.156 -N -f` allowed the Typer CLI python script running locally to speak to `nats://127.0.0.1:4223`, wrapping the raw CNATS payload in the encrypted SSH channel and successfully piercing the IPv4 firewall to access the JetStream cluster health pulse.
