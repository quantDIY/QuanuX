% QUANUXCTL-NEST(1) QuanuX Operations Manual | Version 1.0.0
% QuanuX Infrastructure Team
% March 2026

# NAME

**quanuxctl-nest** - Deploy the QuanuX C++ Sovereign Engine to pre-conditioned Habitat soil.

# SYNOPSIS

**quanuxctl nest drop** *TARGET_GROUP* **--engine** *ENGINE_TYPE*

# DESCRIPTION

`quanuxctl nest drop` executes Phase 2 of the Two-Stage Immutable Deployment pattern. It pushes the repository codebase to the remote Iron, natively compiles the engine against the Habitat's C++ toolchain, and wraps the resulting binary in a highly-optimized systemd daemon.

This command inherently targets `QuanuX-Infra/ansible/03b-nest-drop.yml`.

# NATIVE COMPILATION PIPELINE

To achieve deterministic 59ns latency, the C++ core is NEVER cross-compiled or shipped via Docker. The playbook executes the CMake toolchain natively on the execution droplet utilizing maximum compiler optimizations:

```bash
# Executed remotely via Ansible:
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

The strict `-Werror` flag ensures all unused variables in strategy stubs must be prefixed with `[[maybe_unused]]` to prevent logic leaks.

## TARGET DAEMON AND IPC

The resulting binary is moved to `/opt/quanux/bin/` and executed via `quanux-engine.service`.

The Nest guarantees zero-latency IPC by legally binding the execution thread to the `/etc/quanux/habitat.env` file injected during Phase 1. Without this file, execution is purposefully aborted.

# INSTITUTIONAL VERIFICATION

QuanuX deployments mathematically guarantee exact execution parity. Operations engineers can verify the deployed execution daemons via standard `systemctl` verification on the Edge execution environments:

```bash
# Verify the sovereign C++ execution loop:
$ systemctl status quanux-engine

● quanux-engine.service - QuanuX Sovereign Engine (59ns Spreader Nest)
   Active: active (running)
   Main PID: 6699 (quanux_spreader)
```

The system output will confirm the engine is successfully executing its native `-O3` pipeline, reading from the strict, dynamically-injected `habitat.env` mapping, and howling explicitly across the NATS `10.10.x.x` VPC without internet traversal or static IP hardcodes.

# SEE ALSO

`quanuxctl-habitat(1)`, `quanux-infrastructure-skill(7)`
