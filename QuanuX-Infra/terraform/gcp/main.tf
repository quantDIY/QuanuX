terraform {
  required_providers {
    google = {
      source  = "hashicorp/google"
      version = "~> 5.0"
    }
  }
}

provider "google" {
  credentials = file(var.gcp_credentials_file)
  project     = var.project_id
  region      = var.region
  zone        = var.zone
}

# ---------------------------------------------------------
# The Isolated Matrix VPC
# ---------------------------------------------------------
resource "google_compute_network" "quanux_matrix" {
  name                    = "quanux-vpc-matrix"
  auto_create_subnetworks = false
  description             = "Absolute isolated global mesh routing for QuanuX nodes."
}

resource "google_compute_subnetwork" "quanux_matrix_sub" {
  name          = "quanux-vpc-matrix-sub"
  ip_cidr_range = "10.10.10.0/24"
  region        = var.region
  network       = google_compute_network.quanux_matrix.id
}

# ---------------------------------------------------------
# The Paranoia Firewall (Strict Zero-Trust Perimeter)
# ---------------------------------------------------------
resource "google_compute_firewall" "paranoia_ssh_admin" {
  name    = "quanux-paranoia-ssh"
  network = google_compute_network.quanux_matrix.name

  allow {
    protocol = "tcp"
    ports    = ["22"]
  }

  source_ranges = [var.admin_ip]
  target_tags   = ["quanux-node"]
}

resource "google_compute_firewall" "paranoia_internal_mesh" {
  name    = "quanux-paranoia-internal"
  network = google_compute_network.quanux_matrix.name

  allow {
    protocol = "tcp"
    ports    = ["1-65535"]
  }
  allow {
    protocol = "udp"
    ports    = ["1-65535"]
  }
  allow {
    protocol = "icmp"
  }

  source_ranges = ["10.10.10.0/24"]
  target_tags   = ["quanux-node"]
}

# ---------------------------------------------------------
# Local Variable Defaults
# ---------------------------------------------------------
locals {
  default_machine_type = "e2-medium"     # Roughly equivalent to s-2vcpu-4gb
  nexus_machine_type   = "e2-standard-2" # Roughly equivalent to s-4vcpu-8gb
  annex_machine_type   = "c2-standard-4" # Compute-optimized explicitly requested
  ubuntu_image         = "ubuntu-os-cloud/ubuntu-2404-noble-amd64-v20240924"
  # Fetch from tf variables for standard dynamic injection map
  ssh_metadata = length(var.ssh_keys) > 0 ? {
    "ssh-keys" = join("\n", [for key in var.ssh_keys : "quanux:${key}"])
  } : {}
}

# ---------------------------------------------------------
# Tier 1 Sentinel (Observability: OpenSearch Ledger)
# ---------------------------------------------------------
resource "google_compute_instance" "panopticon_ledger" {
  name         = "quanux-panopticon-ledger"
  machine_type = local.nexus_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-panopticon"]

  boot_disk {
    initialize_params {
      image = local.ubuntu_image
      size  = 50
    }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {
      # Ephemeral public IP to match DO topology
    }
  }
  metadata = local.ssh_metadata
}

# ---------------------------------------------------------
# Tier 1 Sentinel (Observability: ValKey & Python Shadow Node)
# ---------------------------------------------------------
resource "google_compute_instance" "panopticon_buffer" {
  name         = "quanux-panopticon-buffer"
  machine_type = local.default_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-panopticon"]

  boot_disk {
    initialize_params {
      image = local.ubuntu_image
      size  = 30
    }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# ---------------------------------------------------------
# Tier 2 Aleph Protocol
# ---------------------------------------------------------
resource "google_compute_instance" "panopticon_forge" {
  name         = "quanux-panopticon-forge"
  machine_type = local.default_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-panopticon", "quanux-aleph"]

  boot_disk {
    initialize_params { image = local.ubuntu_image }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

resource "google_compute_instance" "panopticon_oracle" {
  name         = "quanux-panopticon-oracle"
  machine_type = local.nexus_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-panopticon", "quanux-aleph"]

  boot_disk {
    initialize_params { image = local.ubuntu_image }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

resource "google_compute_instance" "panopticon_nexus" {
  name         = "quanux-panopticon-nexus"
  machine_type = local.nexus_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-panopticon", "quanux-aleph"]

  boot_disk {
    initialize_params { image = local.ubuntu_image }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# ---------------------------------------------------------
# Tier 4 Nests (The Edge Sovereign Engines)
# ---------------------------------------------------------
resource "google_compute_instance" "edge_nyc" {
  name         = "quanux-edge-nyc"
  machine_type = local.default_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-edge"]

  boot_disk {
    initialize_params { image = local.ubuntu_image }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

resource "google_compute_instance" "edge_nyc_2" {
  name         = "quanux-edge-nyc-2"
  machine_type = local.default_machine_type
  zone         = var.zone
  tags         = ["quanux-node", "quanux-edge"]

  boot_disk {
    initialize_params { image = local.ubuntu_image }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# ---------------------------------------------------------
# The Data Lake (QuanuX-Annex Ingestion & Storage)
# ---------------------------------------------------------
resource "google_compute_instance" "quanux_annex_node" {
  name         = "quanux-annex-ingestion-01"
  machine_type = local.annex_machine_type # c2-standard-4 natively binds to dedicated compute threads
  zone         = var.zone
  tags         = ["quanux-node", "quanux-annex", "high-frequency"]

  boot_disk {
    initialize_params {
      image = local.ubuntu_image
      size  = 100
      type  = "pd-ssd"
    }
  }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  
  # Binding Phase 2 Service Account to VM Instance to let it natively query BigQuery and write to GCS without API keys 
  service_account {
    email  = "quanux-annex-node@${var.project_id}.iam.gserviceaccount.com"
    scopes = ["cloud-platform"]
  }

  metadata = local.ssh_metadata
}

# ---------------------------------------------------------
# Vault Storage: Deep Lake Bucket
# ---------------------------------------------------------
resource "google_storage_bucket" "quanux_zarr_vault" {
  name          = "quanux-deep-lake-${var.project_id}"
  location      = "US" # multi-region equivalent
  force_destroy = true
  
  uniform_bucket_level_access = true
}
