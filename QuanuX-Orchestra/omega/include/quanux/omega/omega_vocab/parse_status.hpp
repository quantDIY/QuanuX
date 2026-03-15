#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class ParseStatus : uint8_t {
    Unknown = 0,
    Success = 1,
    Warning = 2,
    Error = 3
};

} // namespace vocab
} // namespace omega
} // namespace quanux
