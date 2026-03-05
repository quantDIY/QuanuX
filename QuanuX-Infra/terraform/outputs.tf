output "quanux_panopticon_ledger_public_ip" {
  value = digitalocean_droplet.panopticon_ledger.ipv4_address
}

output "quanux_panopticon_ledger_internal_ip" {
  value = digitalocean_droplet.panopticon_ledger.ipv4_address_private
}

output "quanux_panopticon_buffer_public_ip" {
  value = digitalocean_droplet.panopticon_buffer.ipv4_address
}

output "quanux_panopticon_buffer_internal_ip" {
  value = digitalocean_droplet.panopticon_buffer.ipv4_address_private
}

output "quanux_edge_nyc_public_ip" {
  value = digitalocean_droplet.edge_nyc.ipv4_address
}

output "quanux_edge_nyc_internal_ip" {
  value = digitalocean_droplet.edge_nyc.ipv4_address_private
}

output "quanux_edge_nyc_2_public_ip" {
  value = digitalocean_droplet.edge_nyc_2.ipv4_address
}

output "quanux_edge_nyc_2_internal_ip" {
  value = digitalocean_droplet.edge_nyc_2.ipv4_address_private
}

# ---------------------------------------------------------
# Aleph Protocol IPs
# ---------------------------------------------------------
output "quanux_panopticon_forge_public_ip" {
  value = digitalocean_droplet.panopticon_forge.ipv4_address
}

output "quanux_panopticon_forge_internal_ip" {
  value = digitalocean_droplet.panopticon_forge.ipv4_address_private
}

output "quanux_panopticon_vault_public_ip" {
  value = digitalocean_droplet.panopticon_vault.ipv4_address
}

output "quanux_panopticon_vault_internal_ip" {
  value = digitalocean_droplet.panopticon_vault.ipv4_address_private
}

output "quanux_panopticon_oracle_public_ip" {
  value = digitalocean_droplet.panopticon_oracle.ipv4_address
}

output "quanux_panopticon_oracle_internal_ip" {
  value = digitalocean_droplet.panopticon_oracle.ipv4_address_private
}

output "quanux_panopticon_nexus_public_ip" {
  value = digitalocean_droplet.panopticon_nexus.ipv4_address
}

output "quanux_panopticon_nexus_internal_ip" {
  value = digitalocean_droplet.panopticon_nexus.ipv4_address_private
}
