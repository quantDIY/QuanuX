---
name: Network Infrastructure Mandate
description: The foundational documentation mandate for all networking and infrastructure configuration files in QuanuX.
---
# Network Infrastructure Mandate

## Context & Rules
- **1. THE OS VERIFICATION MANDATE**: All networking infrastructure files, deployment scripts, and architecture markdown documents MUST contain explicit 'OS-specific Notes' detailing the precise operating system and LTS version against which they were verified (e.g., Ubuntu 22.04 LTS).
- **2. Rationale**: Networking stacks, NAT resolution, and IPv6 bindings change frequently between OS distributions and major versions. Specifying the verified OS eliminates wasteful re-testing of known physics barriers by future AI agents and human engineers.
- **3. Requirement**: When editing or generating any file related to UFW, VirtualBox NATs, NATS Server configurations, or libcurl transport layers, agents must explicitly append an 'OS Verification Note'.
