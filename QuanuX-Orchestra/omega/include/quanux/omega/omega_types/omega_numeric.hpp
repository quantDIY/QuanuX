#pragma once

namespace quanux {
namespace omega {
namespace types {

// ============================================================================
// TEMPORARY PLACEHOLDER TYPES
// ============================================================================
// WARNING: PROVISIONAL NUMERIC TYPES
// These aliases encapsulate the current usage of 'double' for numeric fields 
// across QuanuX-Omega to prevent naked primitive sprawl, but solving 
// the underlying numeric representation is an active hardening item.
//
// These types MUST NOT be treated as implying accounting-grade or 
// reconciliation-grade precision. They are placeholders.
//
// Future contract hardening will likely replace the underlying type
// (e.g., precise decimal structs, int64 mantissa bounds) without breaking 
// Omega semantics or the adapter mappers utilizing these aliases.
using Price = double;
using Quantity = double;

} // namespace types
} // namespace omega
} // namespace quanux
