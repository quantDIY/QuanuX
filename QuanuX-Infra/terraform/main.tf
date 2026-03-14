terraform {
  required_providers {
    digitalocean = {
      source  = "digitalocean/digitalocean"
      version = "~> 2.0"
    }
  }
}

provider "digitalocean" {
  token = var.do_token
}

# The Isolated Matrix VPC (No public subnet cross-talk)
data "digitalocean_vpc" "quanux_matrix" {
  name = "quanux-vpc-matrix"
}

# Tier 1 Sentinel (Observability: OpenSearch Ledger)
resource "digitalocean_droplet" "panopticon_ledger" {
  name     = "quanux-panopticon-ledger"
  region   = var.region
  size     = "s-4vcpu-8gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon"]
}

# Tier 1 Sentinel (Observability: ValKey & Python Shadow Node)
resource "digitalocean_droplet" "panopticon_buffer" {
  name     = "quanux-panopticon-buffer"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon"]
}

# Tier 2 Aleph Protocol (Forge: Ingestion/Routing)
resource "digitalocean_droplet" "panopticon_forge" {
  name     = "quanux-panopticon-forge"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon", "quanux-aleph"]
}

# Tier 2 Aleph Protocol (Vault: Cold Storage/MinIO S3)
resource "digitalocean_droplet" "panopticon_vault" {
  name     = "quanux-panopticon-vault"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon", "quanux-aleph"]
}

# Tier 2 Aleph Protocol (Oracle: Analytical Compute/DuckDB)
resource "digitalocean_droplet" "panopticon_oracle" {
  name     = "quanux-panopticon-oracle"
  region   = var.region
  size     = "s-4vcpu-8gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon", "quanux-aleph"]
}

# Tier 2 Aleph Protocol (Nexus: Hasura Supergraph API)
resource "digitalocean_droplet" "panopticon_nexus" {
  name     = "quanux-panopticon-nexus"
  region   = var.region
  size     = "s-4vcpu-8gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon", "quanux-aleph"]
}

# Tier 4 Nests (The Edge Sovereign Engines)
resource "digitalocean_droplet" "edge_nyc" {
  name     = "quanux-edge-nyc"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-edge"]
}

resource "digitalocean_droplet" "edge_nyc_2" {
  name     = "quanux-edge-nyc-2"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = data.digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-edge"]
}

# ---------------------------------------------------------
# The Data Lake (QuanuX-Annex Ingestion & Storage)
# ---------------------------------------------------------

# THE OBJECT STORAGE (ZARR TARGET)
resource "digitalocean_spaces_bucket" "quanux_zarr_vault" {
  name   = "quanux-telemetry-zarr-vault"
  region = var.region
  acl    = "private"
  
  # Enforcing lifecycle rules to clear out orphaned blosc2 chunks if flusher fails
  lifecycle_rule {
    enabled = true
    abort_incomplete_multipart_upload_days = 1
  }
}

# THE DEDICATED SILICON (THE PARALLEL FORK HOST)
resource "digitalocean_droplet" "quanux_annex_node" {
  name     = "quanux-annex-ingestion-01"
  region   = var.region
  image    = "ubuntu-24-04-x64"
  
  # CRITICAL PHYSICS: Compute-Optimized, 2 Dedicated vCPUs
  # This size slug guarantees the hypervisor will not steal our clock cycles.
  # Core 1: Hasura Read Path | Core 2: NATS Write Path
  size     = "c-2" 
  
  vpc_uuid   = data.digitalocean_vpc.quanux_matrix.id
  monitoring = true
  ipv6       = false

  # SSH keys mapped from your deployment environment
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-annex", "high-frequency"]
}

# ---------------------------------------------------------
# The Paranoia Firewall (Strict Zero-Trust Perimeter)
# ---------------------------------------------------------
resource "digitalocean_firewall" "paranoia" {
  name = "quanux-paranoia-firewall"

  droplet_ids = [
    digitalocean_droplet.panopticon_ledger.id,
    digitalocean_droplet.panopticon_buffer.id,
    digitalocean_droplet.panopticon_forge.id,
    digitalocean_droplet.panopticon_vault.id,
    digitalocean_droplet.panopticon_oracle.id,
    digitalocean_droplet.panopticon_nexus.id,
    digitalocean_droplet.edge_nyc.id,
    digitalocean_droplet.edge_nyc_2.id,
    digitalocean_droplet.quanux_annex_node.id
  ]

  # INBOUND: Deny all EXCEPT specific Admin IP and 10.10.10.0/24 Mesh chatter
  inbound_rule {
    protocol         = "tcp"
    port_range       = "22"
    source_addresses = [var.admin_ip]
  }

  inbound_rule {
    protocol         = "tcp"
    port_range       = "1-65535"
    source_addresses = [data.digitalocean_vpc.quanux_matrix.ip_range]
  }

  inbound_rule {
    protocol         = "udp"
    port_range       = "1-65535"
    source_addresses = [data.digitalocean_vpc.quanux_matrix.ip_range]
  }

  # OUTBOUND: Allow everything (Node OS updates)
  outbound_rule {
    protocol              = "tcp"
    port_range            = "1-65535"
    destination_addresses = ["0.0.0.0/0", "::/0"]
  }
  outbound_rule {
    protocol              = "udp"
    port_range            = "1-65535"
    destination_addresses = ["0.0.0.0/0", "::/0"]
  }
  outbound_rule {
    protocol              = "icmp"
    destination_addresses = ["0.0.0.0/0", "::/0"]
  }
}
