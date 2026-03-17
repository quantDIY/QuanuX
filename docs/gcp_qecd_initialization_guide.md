# QuanuX Enterprise Cloud Deployment (QECD): GCP Initialization Guide

**Target Audience:** QuanuX Operators deploying to Google Cloud Platform for the first time.  
**Objective:** Install the Google Cloud CLI, authenticate, and run the automated QECD Substrate Script to provision a fresh, least-privilege GCP Project.

---

## Phase 1: Environment Preparation (The gcloud CLI)

To interact with Google Cloud from your terminal, you must install the Google Cloud Command Line Interface (CLI).

### 1. Install the CLI (macOS / Linux)
Open your terminal and execute the official installation script:
```bash
curl https://sdk.cloud.google.com | bash
```

**Note:** During installation, it will ask if you want to modify your profile to update your `$PATH`. Type `Y` and press `Enter`.

**Troubleshooting: "Permission Denied" Error**  
If the installation crashes with a `Permission denied` error regarding `~/.config/gcloud`, it means your local config folder is locked by root. Run this command to reclaim ownership, then re-run the installation script:
```bash
sudo chown -R $(whoami) ~/.config
```

### 2. Inject the CLI into your Path
To ensure your current terminal window recognizes the new `gcloud` command, run:
```bash
source ~/google-cloud-sdk/path.zsh.inc
source ~/google-cloud-sdk/completion.zsh.inc
```
*(If you are using bash instead of zsh, replace `.zsh.inc` with `.bash.inc`)*

---

## Phase 2: Authentication & Billing

### 1. Authenticate your Terminal
Link your local terminal to your Google account:
```bash
gcloud auth login
```
This will open a browser window. Sign in with the Google Account that will own the QuanuX deployment and click "Allow."

### 2. Locate your Billing Account ID
Before running the initialization script, you must tell GCP how to pay for the infrastructure.
1. Go to the [Google Cloud Console Billing Page](https://console.cloud.google.com/billing).
2. Look for the Billing account ID (it will look like `012345-6789AB-CDEF01`). Copy this value.

---

## Phase 3: The QECD Substrate Initialization

We do not use a single "God Key" for QuanuX. The following script automates the creation of a new GCP Project, enables the required APIs, and creates three strictly scoped Service Accounts:
- **TF Provisioner**: For Terraform to build the infrastructure.
- **Annex Node Identity**: For the VMs to write to the Sovereign Vault and query BigQuery.
- **STONITH Executor**: A hyper-restricted account solely for High-Availability failovers.

### 1. Execute the Initialization Script
Copy the entire block of code below. Paste it into your terminal, but do not press Enter yet.  
Replace `YOUR_BILLING_ACCOUNT_ID` with the ID you copied in Phase 2. Once updated, press Enter.

```bash
cat << 'EOF' > qecd_iam_init.sh
#!/usr/bin/env bash
# QuanuX Enterprise Cloud Deployment (QECD) - Initial IAM Stub

PROJECT_ID="quanux-qecd-prod-001"
BILLING_ACCOUNT_ID="YOUR_BILLING_ACCOUNT_ID" 

echo -e "\n[1/5] Creating the QECD Project..."
gcloud projects create $PROJECT_ID --name="QuanuX QECD Prod"
gcloud config set project $PROJECT_ID
gcloud beta billing projects link $PROJECT_ID --billing-account $BILLING_ACCOUNT_ID

echo -e "\n[2/5] Enabling Required GCP APIs..."
gcloud services enable compute.googleapis.com \
                       storage.googleapis.com \
                       bigquery.googleapis.com \
                       iam.googleapis.com \
                       cloudresourcemanager.googleapis.com

echo -e "\n[3/5] Creating the Terraform Provisioner SA..."
gcloud iam service-accounts create tf-provisioner \
    --description="Terraform infrastructure provisioner" \
    --display-name="TF Provisioner"

gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:tf-provisioner@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/compute.admin"
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:tf-provisioner@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/storage.admin"
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:tf-provisioner@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/bigquery.admin"
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:tf-provisioner@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/iam.serviceAccountUser"

echo -e "\n[4/5] Creating the QuanuX Annex Node SA..."
gcloud iam service-accounts create quanux-annex-node \
    --description="Identity for the QuanuX Annex VMs" \
    --display-name="Annex Node Identity"

gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:quanux-annex-node@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/storage.objectAdmin"
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:quanux-annex-node@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/bigquery.dataEditor"
gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:quanux-annex-node@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/bigquery.jobUser"

echo -e "\n[5/5] Creating the STONITH HA Executor SA..."
gcloud iam service-accounts create quanux-stonith \
    --description="Highly restricted SA solely for triggering OOB power-kills" \
    --display-name="STONITH Executor"

gcloud projects add-iam-policy-binding $PROJECT_ID \
    --member="serviceAccount:quanux-stonith@${PROJECT_ID}.iam.gserviceaccount.com" \
    --role="roles/compute.instanceAdmin.v1" 

echo -e "\n[SUCCESS] QECD IAM Substrate Initialization Complete."
EOF

chmod +x qecd_iam_init.sh
./qecd_iam_init.sh
```

---

## Phase 4: Secure the Provisioner Key

Terraform requires a physical key file to authenticate as the `tf-provisioner` and build your infrastructure. Run this final command to generate the key and store it securely in a hidden QuanuX directory on your machine.

```bash
mkdir -p ~/.quanux/keys && \
gcloud iam service-accounts keys create ~/.quanux/keys/qecd-tf-provisioner.json \
    --iam-account=tf-provisioner@quanux-qecd-prod-001.iam.gserviceaccount.com
```

**Next Steps:** You are now ready to proceed to Phase 3 and execute the Terraform provisioning via `quanuxctl`.
