// QuanuX Observability Plane - CNATS C++20 Publisher
// Rule: Zero-copy FlatBuffer manipulation and exclusive usage of CNATS C Client.

#pragma once

#include <nats/nats.h>
#include <flatbuffers/flatbuffers.h>
#include <span>
#include <stdexcept>
#include <chrono>

// Assumes the flatc generated headers are available
#include "../schemas/telemetry_generated.h"

namespace QuanuX::Observability {

    class TelemetryPublisher {
    public:
        // C++20: Explicit constructor avoiding implicit conversions
        explicit TelemetryPublisher(natsConnection* connection, const char* default_subject) noexcept
            : nc_{connection}, subject_{default_subject} {
            // In a production environment, connection state would be strictly monitored via callbacks
        }

        // Delete copy semantics to enforce single-ownership over publishing mechanics
        TelemetryPublisher(const TelemetryPublisher&) = delete;
        TelemetryPublisher& operator=(const TelemetryPublisher&) = delete;

        // C++20: Utilize concepts or inline requires (simplified here for brevity)
        // Publish a fully constructed FlatBuffer Envelope
        auto publish(const flatbuffers::FlatBufferBuilder& builder) const -> natsStatus {
            const auto* buffer_pointer = builder.GetBufferPointer();
            const auto buffer_size = builder.GetSize();

            // Zero-copy mandate: We pipe the raw FlatBuffer memory directly into the CNATS C client
            return natsConnection_Publish(nc_, subject_, reinterpret_cast<const void*>(buffer_pointer), buffer_size);
        }

        // Utility: High-performance execution dispatch
        auto dispatch_execution(
            uint64_t trade_id,
            uint32_t symbol_id,
            QuanuX::Telemetry::ExchangeCode exchange,
            int64_t price,
            int64_t quantity,
            QuanuX::Telemetry::Side side,
            uint64_t latency_ns) const -> natsStatus {
            
            flatbuffers::FlatBufferBuilder builder(256); // Pre-reserve typical envelope size
            
            auto now_ns = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());

            auto exec_offset = QuanuX::Telemetry::CreateTradeExecution(
                builder,
                trade_id,
                symbol_id,
                exchange,
                price,
                quantity,
                side,
                now_ns,
                latency_ns
            );

            auto envelope_offset = QuanuX::Telemetry::CreateTelemetryEnvelope(
                builder,
                now_ns,
                QuanuX::Telemetry::Payload_TradeExecution,
                exec_offset.Union()
            );

            builder.Finish(envelope_offset);
            
            return publish(builder);
        }

    private:
        natsConnection* nc_;
        const char* subject_;
    };

} // namespace QuanuX::Observability
