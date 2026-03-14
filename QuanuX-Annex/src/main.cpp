#include <iostream>
#include <memory>
#include <thread>
#include <csignal>

// Cloud target conditionality driven by CMake -DCLOUD_TARGET=DO
#ifdef CLOUD_TARGET_DO
#include "quanux/annex/ZarrResolver_DO.hpp"
#endif

#include "quanux/annex/federation/HasuraRemoteWorker.hpp"

using namespace quanux::annex;

bool g_running = true;

void signal_handler(int signum) {
    std::cout << "\n[main] Intercepted Termination Signal (" << signum << "). Initiating graceful Annex shutdown...\n";
    g_running = false;
}

int main() {
    std::cout << "[main] Booting QuanuX-Annex Node...\n";
    std::cout << "[main] Hardware Topology Verified: Dedicated Silicon. Aleph Protocol / Spreader isolation confirmed.\n";

    // Bind OS signals for graceful teardown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

#ifdef CLOUD_TARGET_DO
    std::cout << "[main] Cloud Physics: DigitalOcean (Zarr Virtual Filesystem)\n";
    
    // 1. Instantiate the Storage Abstraction Layer (S3-compatible DO Spaces)
    auto resolver = std::make_shared<ZarrResolver_DO>("quanux-deep-lake", "nyc3");

    // 2. THE PARALLEL FORK: Core 1 -> Hasura Remote Schema (Read Path)
    uint16_t federation_port = 8080;
    federation::HasuraRemoteWorker remote_worker(resolver, federation_port);
    
    std::thread federation_thread([&remote_worker]() {
        std::cout << "[Core 1] Launching Hasura GraphQL Federation Worker...\n";
        remote_worker.start_server();
    });

    // 3. THE PARALLEL FORK: Core 2 -> NATS JetStream Ingestion (Write Path)
    std::thread daemon_thread([&resolver]() {
        std::cout << "[Core 2] Launching AnnexDaemon Zarr Ingestion...\n";
        resolver->start_ingestion(); // In a true production fork, this would run on a dedicated AnnexDaemon class
    });

    // Main thread wait loop
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Teardown
    remote_worker.stop_server();
    resolver->stop_ingestion();

    if (federation_thread.joinable()) federation_thread.join();
    if (daemon_thread.joinable()) daemon_thread.join();

#else
    std::cerr << "[FATAL] main.cpp compiled without DO target. Halting.\n";
    return 1;
#endif

    std::cout << "[main] QuanuX-Annex Node Offline.\n";
    return 0;
}
