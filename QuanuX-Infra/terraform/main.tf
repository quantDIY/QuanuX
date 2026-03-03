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
resource "digitalocean_vpc" "quanux_matrix" {
  name        = "quanux-vpc-matrix"
  region      = var.region
  ip_range    = "10.10.10.0/24"
  description = "Absolute isolated global mesh routing for QuanuX nodes."
}

# Tier 1 Sentinel (Observability: OpenSearch Ledger)
resource "digitalocean_droplet" "panopticon_ledger" {
  name     = "quanux-panopticon-ledger"
  region   = var.region
  size     = "s-4vcpu-8gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon"]
}

# Tier 1 Sentinel (Observability: ValKey & Python Shadow Node)
resource "digitalocean_droplet" "panopticon_buffer" {
  name     = "quanux-panopticon-buffer"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-panopticon"]
}

# Tier 4 Nests (The Edge Sovereign Engines)
resource "digitalocean_droplet" "edge_nyc" {
  name     = "quanux-edge-nyc"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-edge"]
}

resource "digitalocean_droplet" "edge_nyc_2" {
  name     = "quanux-edge-nyc-2"
  region   = var.region
  size     = "s-2vcpu-4gb"
  image    = "ubuntu-24-04-x64"
  vpc_uuid = digitalocean_vpc.quanux_matrix.id
  ssh_keys = var.ssh_keys

  tags = ["quanux-node", "quanux-edge"]
}

# ---------------------------------------------------------
# The Paranoia Firewall (Strict Zero-Trust Perimeter)
# ---------------------------------------------------------
resource "digitalocean_firewall" "paranoia" {
  name = "quanux-paranoia-firewall"

  droplet_ids = [
    digitalocean_droplet.panopticon_ledger.id,
    digitalocean_droplet.panopticon_buffer.id,
    digitalocean_droplet.edge_nyc.id,
    digitalocean_droplet.edge_nyc_2.id
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
    source_addresses = [digitalocean_vpc.quanux_matrix.ip_range]
  }

  inbound_rule {
    protocol         = "udp"
    port_range       = "1-65535"
    source_addresses = [digitalocean_vpc.quanux_matrix.ip_range]
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
