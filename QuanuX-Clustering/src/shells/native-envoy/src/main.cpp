#include "probe.hpp"
#include <atomic>
#include <emmintrin.h>
#include <iostream>
#include <nats/nats.h>
#include <nlohmann/json.hpp>
#include <pthread.h>
#include <sched.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <unistd.h>

using json = nlohmann::json;

// Global atomic flag to control the spin loop cleanly
std::atomic<bool> g_keep_running{true};

// Function to strictly lock the calling thread to a specific CPU core
bool pin_thread_to_core(int core_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);

  pthread_t current_thread = pthread_self();
  int result =
      pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);

  return result == 0;
}

// Global logger pointer
std::shared_ptr<spdlog::logger> logger;

// Global NATS connection
natsConnection *conn = nullptr;
natsSubscription *sub = nullptr;

// Probe diagnostic runner
Probe probe_runner;

void on_probe_message(natsConnection *nc, natsSubscription *sub, natsMsg *msg,
                      void *closure) {
  (void)nc;
  (void)sub;
  (void)closure;

  logger->info("Received telemetry IPC from Architect.");

  bool fix = false;

  if (natsMsg_GetDataLength(msg) > 0) {
    std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

    try {
      // Strict JSON Parsing for IPC Security
      json j = json::parse(payload);

      if (j.contains("action") && j.contains("service")) {
        std::string action = j["action"];
        std::string svc = j["service"];

        // Only act if the target is us (the envoy)
        if (svc == "envoy") {
          logger->warn("Architect command received: {} {}", action, svc);

          // Strict Action Validation & Kamikaze Execution
          if (action == "restart" || action == "stop") {
            logger->warn("Executing Kamikaze sequence for {}", action);

            // Construct the exact ACK JSON
            json ack;
            ack["status"] = "acknowledged";
            ack["action"] = action;
            ack["target"] = "quanux-envoy";
            std::string ack_str = ack.dump();

            // Publish the ACK back to the Architect's QUANUX.NODE.STATUS
            // channel
            char hostname[256];
            gethostname(hostname, sizeof(hostname));
            std::string reply_subject =
                "QUANUX.NODE.STATUS." + std::string(hostname);
            natsConnection_PublishString(conn, reply_subject.c_str(),
                                         ack_str.c_str());

            // CRITICAL: Flush the NATS buffer to the wire BEFORE the process
            // dies
            natsConnection_Flush(conn);

            if (action == "restart") {
              int ret = std::system("sudo /bin/systemctl restart quanux-envoy");
              if (ret != 0)
                logger->error("FATAL: Kamikaze restart failed with code {}",
                              ret);
            } else if (action == "stop") {
              int ret = std::system("sudo /bin/systemctl stop quanux-envoy");
              if (ret != 0)
                logger->error("FATAL: Kamikaze stop failed with code {}", ret);
            }
          } else if (action == "start") {
            std::system("sudo /bin/systemctl start quanux-envoy");
          } else if (action == "status") {
            // Status is just a regular probe return
          } else {
            logger->error("Unknown telemetry action: {}", action);
          }
        }
      }

      // Check for Auto-Suture flag in JSON
      if (j.contains("fix") && j["fix"] == true) {
        fix = true;
        logger->info("Executing Probe with Auto-Suture (--fix) enabled.");
      }

    } catch (const json::parse_error &e) {
      // Fallback for legacy string probes (e.g. CLI tests sending plain
      // "--fix")
      if (payload.find("--fix") != std::string::npos) {
        fix = true;
        logger->info("Executing Probe with Auto-Suture (--fix) enabled (Legacy "
                     "format).");
      } else {
        logger->error("JSON Parse error on incoming NATS IPC: {}", e.what());
      }
    }
  }

  // Execute the bare-metal probe diagnostics
  ProbeResult res = probe_runner.execute(fix);

  // Format response JSON
  char hostname[256];
  gethostname(hostname, sizeof(hostname));

  std::string response =
      "{ \"bone\": " + std::string(res.bone_ok ? "true" : "false") +
      ", \"nerve\": " + std::string(res.nerve_ok ? "true" : "false") +
      ", \"soul\": " + std::string(res.soul_ok ? "true" : "false") +
      ", \"exit_code\": " + std::to_string(res.overall_exit_code) + " }";

  std::string reply_subject = "QUANUX.NODE.STATUS." + std::string(hostname);

  natsStatus s = natsConnection_PublishString(conn, reply_subject.c_str(),
                                              response.c_str());
  if (s != NATS_OK) {
    logger->error("Failed to publish telemetry status: {}",
                  natsStatus_GetText(s));
  } else {
    logger->info("Published status to {}", reply_subject);
  }

  natsMsg_Destroy(msg);
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  // Initialize spdlog Black Box Historian
  try {
    logger = spdlog::rotating_logger_mt("envoy", "/var/log/quanux/envoy.log",
                                        10 * 1024 * 1024, 3);
    spdlog::set_default_logger(logger);
    logger->info("Native Envoy initializing...");
  } catch (const spdlog::spdlog_ex &ex) {
    std::cerr << "Log init failed: " << ex.what() << std::endl;
    return 1;
  }

  // 1. PIN THE ENVOY TO THE MANAGEMENT CORE (Core 0)
  if (!pin_thread_to_core(0)) {
    logger->critical("FATAL: Failed to pin Native Envoy to Core 0. Aborting.");
    return 1;
  }

  logger->info("[ENV] Native Envoy online. Pinned to Core 0.");

  // natsOptions (we would use natsOptions_SetEventLoopAffinity if our libnats-c
  // version supports it, otherwise we rely on CNATS handling or explicit thread
  // creation configuration)
  natsOptions *opts = nullptr;
  natsStatus s = natsOptions_Create(&opts);
  if (s == NATS_OK) {
    // Example URL; in a real cluster this comes from an env var
    natsOptions_SetURL(opts, "nats://127.0.0.1:4222");
  }

  // 2. Initialize CNATS
  s = natsConnection_Connect(&conn, opts);
  if (s != NATS_OK) {
    logger->error("Failed to connect to NATS: {}", natsStatus_GetText(s));
    return 1;
  }

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  std::string test_subject = "QUANUX.NODE.PROBE." + std::string(hostname);

  s = natsConnection_Subscribe(&sub, conn, test_subject.c_str(),
                               on_probe_message, nullptr);
  if (s != NATS_OK) {
    logger->error("Failed to subscribe to NATS heartbeat channel: {}",
                  natsStatus_GetText(s));
    return 1;
  }

  logger->info("[ENV] Watchdog active on {}. Entering C0 State Lock...",
               test_subject);

  // 3. THE WARM CORE SPIN-WAIT (Zero-Yield)
  while (g_keep_running.load(std::memory_order_relaxed)) {
    _mm_pause();
  }

  logger->info("[ENV] Shutting down Native Envoy.");

  if (sub)
    natsSubscription_Destroy(sub);
  if (conn)
    natsConnection_Destroy(conn);
  if (opts)
    natsOptions_Destroy(opts);

  return 0;
}
