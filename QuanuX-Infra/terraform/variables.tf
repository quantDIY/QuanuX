variable "do_token_droplets" {
  description = "DigitalOcean API Token for Droplets/VPC (Injected via Python Keyring)"
  type        = string
  sensitive   = true
}

variable "spaces_access_id" {
  description = "DigitalOcean Spaces Access Key ID"
  type        = string
  sensitive   = true
}

variable "spaces_secret_key" {
  description = "DigitalOcean Spaces Secret Key"
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
