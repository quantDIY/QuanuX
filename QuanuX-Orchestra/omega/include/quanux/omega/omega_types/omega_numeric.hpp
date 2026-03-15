#pragma once

namespace quanux {
namespace omega {
namespace types {

// ============================================================================
// TEMPORARY PLACEHOLDER TYPES
// ============================================================================
// WARNING: 
// These double aliases are strictly provisional for Phase 2 contract hardening.
// They must NOT be treated as the final Omega standard. 
// Floating-point ambiguity becomes dangerous in post-trade/reconciliation.
// These will be replaced by a project-local decimal/fixed-point type wrapper.
// Do not encode double semantics deeply into fixture assumptions.

using Price = double;
using Quantity = double;

} // namespace types
} // namespace omega
} // namespace quanux
