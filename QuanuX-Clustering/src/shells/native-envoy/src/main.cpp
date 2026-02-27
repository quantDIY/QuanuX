#include "probe.hpp"
#include <atomic>
#include <curl/curl.h>
#include <emmintrin.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nats/nats.h>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <pthread.h>
#include <sched.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

#include <sys/stat.h>
#include <thread>

#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#endif

using json = nlohmann::json;

// Global Collision Matrix updated dynamically
std::vector<std::vector<std::string>> COLLISION_MATRIX;

// Global atomic flag to control the spin loop cleanly
std::atomic<bool> g_keep_running{true};

// Helper function to pin a thread to a specific core cleanly (Linux & macOS
// support)
bool pin_thread_to_core(int core_id) {
#ifdef __APPLE__
  thread_affinity_policy_data_t policy = {core_id};
  thread_port_t mach_thread = pthread_mach_thread_np(pthread_self());
  thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                    (thread_policy_t)&policy, 1);
  return true;
#else
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);

  pthread_t current_thread = pthread_self();
  int result =
      pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);

  return result == 0;
#endif
}

// Global logger pointer
std::shared_ptr<spdlog::logger> logger;

// Global NATS connection
natsConnection *conn = nullptr;
natsSubscription *sub = nullptr;
natsSubscription *handshake_sub = nullptr;
natsSubscription *deploy_sub = nullptr;

// Probe diagnostic runner
Probe probe_runner;

size_t curl_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  return fwrite(ptr, size, nmemb, stream);
}

// Memory struct for fetching JSON into memory
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t curl_write_memory_callback(void *contents, size_t size,
                                         size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr)
    return 0;

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

bool sync_collision_matrix() {
  CURL *curl;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = (char *)malloc(1);
  chunk.size = 0;

  bool success = false;
  curl = curl_easy_init();
  if (curl) {
    // Assuming Tier 1 server is on localhost:8080 for simulation or passed via
    // env var
    const char *tier1_url = std::getenv("QUANUX_TIER1_URL");
    if (!tier1_url)
      tier1_url = "http://127.0.0.1:8080";
    std::string url = std::string(tier1_url) + "/configs/collision_matrix.json";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
      try {
        json j = json::parse(chunk.memory);
        if (j.contains("collision_matrix") &&
            j["collision_matrix"].is_array()) {
          COLLISION_MATRIX.clear();
          for (auto &row : j["collision_matrix"]) {
            if (row.is_array()) {
              std::vector<std::string> rule;
              for (auto &item : row)
                rule.push_back(item);
              COLLISION_MATRIX.push_back(rule);
            }
          }
          if (logger)
            logger->info("Successfully synced Collision Matrix from Tier 1.");
          success = true;
        }
      } catch (json::parse_error &e) {
        if (logger)
          logger->error("JSON Parse Error on Collision Matrix: {}", e.what());
      }
    } else {
      if (logger)
        logger->error("Failed to sync Collision Matrix via libcurl.");
    }
    curl_easy_cleanup(curl);
  }
  free(chunk.memory);
  return success;
}

std::string compute_sha256(const std::string &filepath) {
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int lengthOfHash = 0;

  EVP_MD_CTX *context = EVP_MD_CTX_new();
  EVP_DigestInit_ex(context, EVP_sha256(), NULL);

  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    EVP_MD_CTX_free(context);
    return "";
  }

  char buffer[8192];
  while (file.read(buffer, sizeof(buffer))) {
    EVP_DigestUpdate(context, buffer, file.gcount());
  }
  // Read the last chunk
  if (file.gcount() > 0) {
    EVP_DigestUpdate(context, buffer, file.gcount());
  }

  EVP_DigestFinal_ex(context, hash, &lengthOfHash);
  EVP_MD_CTX_free(context);

  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  for (unsigned int i = 0; i < lengthOfHash; ++i) {
    ss << std::setw(2) << (int)hash[i];
  }
  return ss.str();
}

void on_handshake_message(natsConnection *nc, natsSubscription *sub,
                          natsMsg *msg, void *closure) {
  (void)nc;
  (void)sub;
  (void)closure;
  logger->info("Received Handshake Interrogation from Architect.");

  if (natsMsg_GetDataLength(msg) > 0) {
    std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));
    try {
      json j = json::parse(payload);
      if (j.contains("requires") && j["requires"].is_array()) {

        // Read local /etc/quanux/habitat.json
        std::ifstream ifs("/etc/quanux/habitat.json");
        json habitat;
        if (ifs.is_open()) {
          ifs >> habitat;
        } else {
          logger->error("Could not read habitat.json");
          habitat["provides"] = json::array();
        }
        // Check if requires is a subset of provides
        bool compatible = true;
        std::string nack_reason = "";
        if (habitat.contains("provides") && habitat["provides"].is_array()) {
          auto provides = habitat["provides"];
          for (auto &req : j["requires"]) {
            bool found = false;
            for (auto &prov : provides) {
              if (prov == req) {
                found = true;
                break;
              }
            }
            if (!found) {
              compatible = false;
              nack_reason = "Missing capability: " + req.get<std::string>();
              break;
            }
          }
        } else {
          compatible = false;
          nack_reason = "Habitat provides no capabilities.";
        }

        // Universal Collision Detection
        if (compatible) {
          for (const auto &collision_set : COLLISION_MATRIX) {
            int collision_count = 0;
            for (auto &req : j["requires"]) {
              for (auto &col : collision_set) {
                if (req == col)
                  collision_count++;
              }
            }
            if (collision_count > 1) {
              compatible = false;
              nack_reason = "COLLISION DETECTED: Hardware mutually exclusive "
                            "traits requested.";
              logger->error(nack_reason);
              break;
            }
          }
        }

        json reply;
        reply["status"] = compatible ? "ACK" : "NACK";
        reply["habitat"] = habitat;
        if (!compatible)
          reply["reason"] = nack_reason;
        std::string reply_str = reply.dump();

        // Send the reply back to the architect
        const char *reply_subject = natsMsg_GetReply(msg);
        if (reply_subject != nullptr && strlen(reply_subject) > 0) {
          natsConnection_PublishString(conn, reply_subject, reply_str.c_str());
        }
      }
    } catch (const json::parse_error &e) {
      logger->error("JSON Parse error on handshake: {}", e.what());
    }
  }
  natsMsg_Destroy(msg);
}

void on_deploy_message(natsConnection *nc, natsSubscription *sub, natsMsg *msg,
                       void *closure) {
  (void)nc;
  (void)sub;
  (void)closure;
  logger->info("Received OOB DEPLOY Request.");

  if (natsMsg_GetDataLength(msg) > 0) {
    std::string payload(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));
    std::thread([payload]() {
      // 1. Instantly pin this background thread to Core 0
      if (!pin_thread_to_core(0)) {
        if (logger)
          logger->error(
              "FATAL: Failed to pin detached OOB download thread to Core 0.");
      } else {
        if (logger)
          logger->info("[ENV] OOB Download thread successfully chained to "
                       "Management Core 0.");
      }

      try {
        json j = json::parse(payload);
        if (j.contains("url") && j.contains("hash") &&
            j.contains("payload_name")) {
          std::string url = j["url"];
          std::string expected_hash = j["hash"];
          std::string payload_name = j["payload_name"];
          std::string execution_prefix =
              j.contains("execution_prefix")
                  ? j["execution_prefix"].get<std::string>()
                  : "";

          // Payloads directory is pre-provisioned securely by node_init.sh
          std::string out_path = "/opt/quanux/payloads/" + payload_name;
          if (logger)
            logger->info("Downloading {} via libcurl...", out_path);

          CURL *curl;
          FILE *fp;
          CURLcode res;
          curl = curl_easy_init();
          if (curl) {
            fp = fopen(out_path.c_str(), "wb");
            if (fp) {
              curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
              curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
              curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
              res = curl_easy_perform(curl);
              curl_easy_cleanup(curl);
              fclose(fp);

              if (res == CURLE_OK) {
                // Strict Hash Checking natively
                std::string calc_hash = compute_sha256(out_path);
                if (calc_hash == expected_hash) {
                  if (logger)
                    logger->info("Hash Verified. Payload {} Secured.",
                                 calc_hash);
                  // Execute POSIX chmod for explicit execute permission
                  if (chmod(out_path.c_str(), S_IRWXU) != 0) {
                    if (logger)
                      logger->error("FATAL: chmod failed for payload binary.");
                  }

                  // Conditioned SSH Wrapper
                  std::string wrapper_path =
                      "/opt/quanux/payloads/.run_" + payload_name + ".sh";
                  std::ofstream wrapper(wrapper_path);
                  wrapper << "#!/usr/bin/env bash\n";
                  wrapper << "# QuanuX Dynamically Wired Execution Wrapper\n";
                  wrapper << execution_prefix << " " << out_path << " \"$@\"\n";
                  wrapper.close();

                  if (chmod(wrapper_path.c_str(), S_IRWXU) != 0) {
                    if (logger)
                      logger->error("FATAL: chmod failed for exec wrapper.");
                  }
                  if (logger)
                    logger->info("Execution Wrapper Created: {}", wrapper_path);

                } else {
                  if (logger)
                    logger->error("Hash mismatch! Exp: {} Got: {}",
                                  expected_hash, calc_hash);
                  remove(out_path.c_str());
                }
              } else {
                if (logger)
                  logger->error("libcurl download failed.");
              }
            }
          }
        }
      } catch (const json::parse_error &e) {
        if (logger)
          logger->error("JSON Parse error on deploy: {}", e.what());
      }
    }).detach();
  }
  natsMsg_Destroy(msg);
}

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

  // 0. FETCH COLLISION MATRIX (DYNAMIC STATE SYNC)
  logger->info("[ENV] Syncing Collision Matrix from Central Architect...");
  sync_collision_matrix();

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

  std::string handshake_subject =
      "QUANUX.NODE.HANDSHAKE." + std::string(hostname);
  s = natsConnection_Subscribe(&handshake_sub, conn, handshake_subject.c_str(),
                               on_handshake_message, nullptr);
  if (s != NATS_OK) {
    logger->error("Failed to subscribe to Handshake channel: {}",
                  natsStatus_GetText(s));
  } else {
    logger->info("[ENV] Handshake Responder active on {}", handshake_subject);
  }

  std::string deploy_subject = "QUANUX.NODE.DEPLOY." + std::string(hostname);
  s = natsConnection_Subscribe(&deploy_sub, conn, deploy_subject.c_str(),
                               on_deploy_message, nullptr);
  if (s != NATS_OK) {
    logger->error("Failed to subscribe to Deploy channel: {}",
                  natsStatus_GetText(s));
  } else {
    logger->info("[ENV] OOB Deploy Responder active on {}", deploy_subject);
  }

  // 3. THE WARM CORE SPIN-WAIT (Zero-Yield)
  while (g_keep_running.load(std::memory_order_relaxed)) {
    _mm_pause();
  }

  logger->info("[ENV] Shutting down Native Envoy.");

  if (deploy_sub)
    natsSubscription_Destroy(deploy_sub);
  if (handshake_sub)
    natsSubscription_Destroy(handshake_sub);
  if (sub)
    natsSubscription_Destroy(sub);
  if (conn)
    natsConnection_Destroy(conn);
  if (opts)
    natsOptions_Destroy(opts);

  return 0;
}
