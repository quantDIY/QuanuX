variable "project_id" {
  description = "GCP Project ID"
  type        = string
}

variable "region" {
  description = "Primary GCP Region"
  type        = string
  default     = "us-east4"
}

variable "zone" {
  description = "Primary GCP Zone"
  type        = string
  default     = "us-east4-a"
}

variable "ssh_keys" {
  description = "List of SSH Public Keys for the instances"
  type        = list(string)
  default     = []
}

variable "admin_ip" {
  description = "Your Admin IPv4 Address for Firewall Whitelisting"
  type        = string
}

variable "gcp_credentials_file" {
  description = "Path to the GCP Service Account Key JSON"
  type        = string
}
