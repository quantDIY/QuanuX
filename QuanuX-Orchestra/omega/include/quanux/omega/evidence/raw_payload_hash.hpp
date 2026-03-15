#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace evidence {

// Represents a deterministically computed hash of the raw payload
// Used for provenance and integrity validation without storing the full payload
struct RawPayloadHash {
    uint64_t hash_value{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return hash_value != 0;
    }
};

// Simple FNV-1a hash algorithm suitable for compile-time or extremely fast runtime hashing
// of small wire payloads.
[[nodiscard]] constexpr uint64_t compute_fnv1a(const uint8_t* data, std::size_t size) noexcept {
    uint64_t hash = 0xcbf29ce484222325ull;
    const uint64_t prime = 0x100000001b3ull;
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

} // namespace evidence
} // namespace omega
} // namespace quanux
