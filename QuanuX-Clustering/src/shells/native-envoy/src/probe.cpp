#include "probe.hpp"
#include <cstdlib>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <sys/stat.h>

namespace fs = std::filesystem;

ProbeResult Probe::execute(bool fix) {
  ProbeResult result;

  int bone_code = check_bone(fix);
  result.bone_ok = (bone_code == 0);

  int nerve_code = check_nerve(fix);
  result.nerve_ok = (nerve_code == 0);

  int soul_code = check_soul(fix);
  result.soul_ok = (soul_code == 0);

  result.overall_exit_code = std::max({bone_code, nerve_code, soul_code});

  return result;
}

int Probe::check_bone(bool fix) {
  // Check if the QuanuX infrastructure binaries exist
  fs::path bin_path = "/usr/local/bin/quanux-envoy";
  if (fs::exists(bin_path)) {
    return 0; // Bone OK
  }

  // Fallback or secondary check
  fs::path opt_path = "/opt/quanux/bin/set-quanux-node";
  if (fs::exists(opt_path)) {
    return 0; // Bone OK
  }

  spdlog::warn("Bone diagnostics failed: Core binaries not found.");
  if (fix) {
    spdlog::info(
        "Attempting to fix Bone: Not implemented in Native Envoy."); // Typically
                                                                     // node_init.sh
                                                                     // handles
                                                                     // this
  }
  return 2;
}

int Probe::check_nerve(bool fix) {
  // In actual implementation this might ping NATS or check the nats-server
  // status. For Native Envoy, the fact that we are connected to NATS via our
  // global connection is a strong indicator, but here we explicitly check the
  // system service if possible.

  // A simple check is to use systemctl is-active
  int sys_ret = std::system("systemctl is-active --quiet nats-server");
  if (sys_ret == 0) {
    return 0; // Nerve OK
  }

  spdlog::warn("Nerve diagnostics failed: nats-server is not active.");
  if (fix) {
    spdlog::info("Attempting to fix Nerve: Restarting nats-server");
    int fix_ret = std::system("sudo systemctl restart nats-server");
    if (fix_ret == 0)
      return 0;
  }
  return 1;
}

int Probe::check_soul(bool fix) {
  // Check permissions on the node key
  struct stat st;
  if (stat("/etc/quanux/keys/node.nk", &st) == 0) {
    if ((st.st_mode & 0777) == 0600) {
      return 0; // Soul OK
    }
  } else {
    spdlog::warn("Soul diagnostics failed: Key file not found.");
    return 3;
  }

  spdlog::warn("Soul diagnostics failed: Incorrect permissions on node key.");
  if (fix) {
    spdlog::info("Attempting to fix Soul: Setting 0600 on node.nk");
    int fix_ret = std::system("sudo chmod 600 /etc/quanux/keys/node.nk");
    if (fix_ret == 0)
      return 0;
  }

  return 3;
}
