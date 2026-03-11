# QuanuX DEVELOPERS Quickstart

This document strictly defines the Scorched Earth rebuild process for the QuanuX superGraph V12 Matrix infrastructure. By adhering to the Tier-1 philosophy, our infrastructure defines "The Nest" (isolation chambers) and "The Habitat" (underlying OS and networking physics) as pure code. 

If the digital infrastructure needs to be purged, follow this exact sequence to resurrect the 234k msgs/sec telemetry leviathan.

## Prerequisites
- DigitalOcean API keys exported in your local environment.
- SSH keypairs available.
- Ansible `vault_secrets.yml` file explicitly declared in `.gitignore` (Never commit the castle keys).

## 1. The Scorched Earth Reset (Terraform)
Destroy and recreate the underlying silicon across the DigitalOcean VPC backbone.

```bash
cd QuanuX-Infra/terraform/
terraform destroy -auto-approve
terraform plan -out=v12_matrix.tfplan
terraform apply "v12_matrix.tfplan"
```
*At this stage, raw Ubuntu 24.04 LTS droplets have been provisioned across the `10.10.10.0/24` subnet.*

## 2. QuanuX-Orchestra Compilation (The Naming Registry)
Before conditioning the Habitat, compile the Universal Naming Registry natively. The Standardizer CLI utilizes `pugixml` directly embedded in the C++ tree, enforcing a zero-dependency, JVM-free build perimeter. It defines the exact memory layout for all Tier-2 algorithms.

**Fail-Fast Execution:** The `quanuxctl orchestra bootstrap` fetcher operates under a strict `sys.exit(1)` fail-fast protocol. It will pull the true 7.6MB instance payload. Network drops will fatally crash the pipeline without silent fallbacks.

**FIGI Setup Context:** Developers must provide a localized `venues/<venue>/figi_map.csv` mapping file. The compiler consumes this literal to generate the zero-latency `constexpr` translation arrays natively.

**FIGI Setup Context:** Developers must provide a localized `venues/<venue>/figi_map.csv` mapping file. The compiler consumes this literal to generate the zero-latency `constexpr` translation arrays natively.

```bash
cd QuanuX-Orchestra/
mkdir build && cd build
cmake ..
make -j$(nproc)
```
*At this stage, the `standardizer_cli` binary is available to natively parse raw FIX `.xml` dictionaries and output `constexpr` bridges and Cython `.pyx` bindings.*

### The Downstream Contract (CMake Integration)
Any Spreader algorithm or downstream execution node structurally depends on the Universal Naming Registry. Instead of relying on static git artifacts, downstream components integrate Orchestra directly into their `CMakeLists.txt` using mandatory pre-build hooks.

```cmake
# Execute the Pipeline as a strict pre-build step
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/generated/constants.hpp
    COMMAND quanuxctl orchestra compile --venue ibkr_onixs
    COMMAND quanuxctl orchestra verify
    DEPENDS ${CMAKE_SOURCE_DIR}/QuanuX-Orchestra/repository/FIX.latest.xml
    COMMENT "Enforcing QuanuX-Orchestra Source-of-Truth Doctrine..."
)

# Standardizer outputs become the explicit target dependency
add_custom_target(OrchestraMatrix DEPENDS ${CMAKE_BINARY_DIR}/generated/constants.hpp)
add_dependencies(Tier2_SpreaderEngine OrchestraMatrix)
```
If the schema fingerprint diverges, `quanuxctl orchestra verify` violently crashes the `make` process.

## 3. Habitat Conditioning (The PEP 668 Bypass)
Ubuntu 24.04 aggressively prevents global pip installations to protect the OS kernel (PEP 668). We must establish the pristine "Habitat" by instructing Ansible to create isolated `python3-venv` environments, circumventing the package manager prior to application deployment.

```bash
cd ../ansible/
ansible-playbook -i dynamic_inventory.py 04-aleph-habitat.yml
```
*At this stage, the VPC is securely firewalled locking ingress ports locally, and the pristine Python Nests have been established on the raw metal.*

## 4. The Aleph Protocol (Application Deployment)
Deploy the distributed microservices. This playbook locks in the NATS Ingestion Crucible, the GreptimeDB storage routers, and executes the "Midnight Pivot"—injecting bare-metal PostgreSQL 16 directly via APT and enabling the `pgcrypto` engine.

```bash
ansible-playbook -i dynamic_inventory.py --extra-vars="@vault_secrets.yml" 05-aleph-protocol.yml
```

### Verification Checks
1. **The Ingestion Buffer:** Ensure NATS is running on the `panopticon-buffer` node on port 4222.
2. **The Ephemeral Cache:** Trigger the C-binding script on `panopticon-oracle`: `python3 materialize_bridge.py`
3. **The Presentation Layer:** Read the Hasura GraphQL Supergraph natively from the `panopticon-nexus` node on port 8080.

A fully deployed architecture will withstand a NATS Publisher stream benchmark of ~228 MiB/second over the `10.10.10.x` network. Any performance drop below 100 MiB/s indicates a loopback abstraction leak.
