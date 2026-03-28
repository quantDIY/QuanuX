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
# Local Variable Defaults (QECD Master Architecture GCP)
# ---------------------------------------------------------
locals {
  # Standardized QECD machine types
  machine_type_standard     = "e2-standard-4"    # Core Control Plane Nodes
  machine_type_highmem      = "n2-standard-8"    # Analytical & Backtesting
  machine_type_highcpu_edge = "c3-highcpu-8"     # Edge Silicon Execution
  machine_type_annex        = "c2-standard-16"   # High-Frequency NATS Ingestion
  machine_type_light        = "e2-standard-2"    # Ledger / Utilities
  
  ubuntu_image = "ubuntu-os-cloud/ubuntu-2404-lts-amd64"
  
  ssh_metadata = length(var.ssh_keys) > 0 ? {
    "ssh-keys" = join("\n", [for key in var.ssh_keys : "quanux:${key}"])
  } : {}
}

# =========================================================
# QECD Master Architecture "Always-On" 9-Node Control Plane
# =========================================================

# 1. Orchestra: Master Saga Coordinator
resource "google_compute_instance" "quanux_orchestra_01" {
  name         = "quanux-orchestra-01"
  machine_type = local.machine_type_standard
  zone         = var.zone
  tags         = ["quanux-node", "quanux-control-plane"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 50 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 2. Server: Central API and User Facing Gateway
resource "google_compute_instance" "quanux_server_01" {
  name         = "quanux-server-01"
  machine_type = local.machine_type_standard
  zone         = var.zone
  tags         = ["quanux-node", "quanux-control-plane"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 50 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 3. Annex: NATS JetStream High-Frequency Ingestion Matrix
resource "google_compute_instance" "quanux_annex_01" {
  name         = "quanux-annex-01"
  machine_type = local.machine_type_annex # c2-standard-16 explicitly binds threads
  zone         = var.zone
  tags         = ["quanux-node", "quanux-data-plane", "high-frequency"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 200; type = "pd-ssd" } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  service_account {
    email  = "quanux-annex-node@${var.project_id}.iam.gserviceaccount.com"
    scopes = ["cloud-platform"]
  }
  metadata = local.ssh_metadata
}

# 4. Search: Semantic Engine (Qdrant & DuckDB)
resource "google_compute_instance" "quanux_search_01" {
  name         = "quanux-search-01"
  machine_type = local.machine_type_standard
  zone         = var.zone
  tags         = ["quanux-node", "quanux-data-plane"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 100; type = "pd-ssd" } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 5. Backtesting: High-RAM Strategy Evaluator (DuckDB/Parquet)
resource "google_compute_instance" "quanux_backtest_01" {
  name         = "quanux-backtest-01"
  machine_type = local.machine_type_highmem
  zone         = var.zone
  tags         = ["quanux-node", "quanux-research"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 500; type = "pd-ssd" } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  service_account {
    email  = "quanux-research-node@${var.project_id}.iam.gserviceaccount.com"
    scopes = ["cloud-platform"]
  }
  metadata = local.ssh_metadata
}

# 6. Ledger: Telemetry & Observability (OpenSearch)
resource "google_compute_instance" "quanux_ledger_01" {
  name         = "quanux-ledger-01"
  machine_type = local.machine_type_light
  zone         = var.zone
  tags         = ["quanux-node", "quanux-observability"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 100 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 7. Vault: Cold Storage Coordinator
resource "google_compute_instance" "quanux_vault_01" {
  name         = "quanux-vault-01"
  machine_type = local.machine_type_light
  zone         = var.zone
  tags         = ["quanux-node", "quanux-storage"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 30 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 8. Nexus: Hasura Supergraph API
resource "google_compute_instance" "quanux_nexus_01" {
  name         = "quanux-nexus-01"
  machine_type = local.machine_type_standard
  zone         = var.zone
  tags         = ["quanux-node", "quanux-control-plane"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 50 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}

# 9. Buffer: Lightning Cache (ValKey)
resource "google_compute_instance" "quanux_buffer_01" {
  name         = "quanux-buffer-01"
  machine_type = local.machine_type_light
  zone         = var.zone
  tags         = ["quanux-node", "quanux-cache"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 30 } }
  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}


# =========================================================
# Tier 4 Nests (Dynamic Execution Fleet)
# =========================================================

# The Spreader Strategy Node - Dedicated Execution 
resource "google_compute_instance" "quanux_exec_spreader_aapl" {
  name         = "quanux-exec-spreader-aapl"
  machine_type = local.machine_type_highcpu_edge
  zone         = var.zone
  tags         = ["quanux-node", "quanux-execution"]

  boot_disk { initialize_params { image = local.ubuntu_image; size = 50 } }

  network_interface {
    network    = google_compute_network.quanux_matrix.id
    subnetwork = google_compute_subnetwork.quanux_matrix_sub.id
    access_config {}
  }
  metadata = local.ssh_metadata
}


# ---------------------------------------------------------
# Vault Storage: Deep Lake Bucket (GCS)
# ---------------------------------------------------------
resource "google_storage_bucket" "quanux_zarr_vault" {
  name          = "quanux-deep-lake-${var.project_id}"
  location      = "US" # multi-region equivalent
  force_destroy = true
  
  uniform_bucket_level_access = true
}
