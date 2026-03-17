output "quanux_panopticon_nexus_public_ip" {
  description = "The public IP address of the Nexus API node"
  value       = google_compute_instance.panopticon_nexus.network_interface[0].access_config[0].nat_ip
}

output "quanux_panopticon_nexus_internal_ip" {
  description = "The internal (VPC) IP address of the Nexus API node"
  value       = google_compute_instance.panopticon_nexus.network_interface[0].network_ip
}

output "quanux_annex_node_public_ip" {
  description = "The public IP address of the QuanuX-Annex ingestion node"
  value       = google_compute_instance.quanux_annex_node.network_interface[0].access_config[0].nat_ip
}

output "quanux_annex_node_internal_ip" {
  description = "The internal (VPC) IP address of the QuanuX-Annex node"
  value       = google_compute_instance.quanux_annex_node.network_interface[0].network_ip
}

output "quanux_zarr_vault_name" {
  description = "Name of the Vault bucket"
  value       = google_storage_bucket.quanux_zarr_vault.name
}

output "quanux_zarr_vault_endpoint" {
  description = "S3-compatible endpoint for Vault bucket"
  value       = google_storage_bucket.quanux_zarr_vault.url
}
