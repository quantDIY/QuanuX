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
