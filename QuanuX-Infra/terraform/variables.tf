variable "do_token" {
  description = "DigitalOcean API Token (Injected by quanuxctl infra auth-shell)"
  type        = string
  sensitive   = true
}

variable "region" {
  description = "Target DigitalOcean deployment region"
  type        = string
  default     = "nyc3"
}

variable "ssh_keys" {
  description = "List of SSH Fingerprints authorized for the Droplets"
  type        = list(string)
  default     = []
}

variable "admin_ip" {
  description = "The single IPv4 address authorized for SSH access through the Paranoia Firewall"
  type        = string
}
