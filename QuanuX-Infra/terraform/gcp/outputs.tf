output "quanux_orchestra_public_ip" {
  description = "The public IP address of the Orchestra Control Node"
  value       = google_compute_instance.quanux_orchestra_01.network_interface[0].access_config[0].nat_ip
}

output "quanux_nexus_public_ip" {
  description = "The public IP address of the Nexus API node"
  value       = google_compute_instance.quanux_nexus_01.network_interface[0].access_config[0].nat_ip
}

output "quanux_annex_public_ip" {
  description = "The public IP address of the QuanuX-Annex ingestion node"
  value       = google_compute_instance.quanux_annex_01.network_interface[0].access_config[0].nat_ip
}

output "quanux_search_public_ip" {
  description = "The public IP address of the Search layer node"
  value       = google_compute_instance.quanux_search_01.network_interface[0].access_config[0].nat_ip
}

output "quanux_exec_spreader_aapl_public_ip" {
  description = "The public IP address of the dedicated execution nest"
  value       = google_compute_instance.quanux_exec_spreader_aapl.network_interface[0].access_config[0].nat_ip
}

output "quanux_zarr_vault_name" {
  description = "Name of the Vault bucket"
  value       = google_storage_bucket.quanux_zarr_vault.name
}

output "quanux_zarr_vault_endpoint" {
  description = "S3-compatible/GCS endpoint for Vault bucket"
  value       = google_storage_bucket.quanux_zarr_vault.url
}
