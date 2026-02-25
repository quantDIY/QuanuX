#pragma once
#ifndef QUANUX_TRANSLATOR_STUB_HPP
#define QUANUX_TRANSLATOR_STUB_HPP

#include <iostream>
#include <string>

namespace quanux {
namespace aether {

/**
 * @brief The Universal Translator (OnixS SDK Interface)
 *
 * This is the pure virtual interface for the "Sanctified Graft".
 * It represents the final membrane boundary between the QuanuX Neural
 * Singularity (16-cycle ingress / 26-cycle egress) and the native binary
 * intelligence of the Exchange matching engines via the OnixS Solarflare
 * Handler.
 */
class UniversalTranslator {
public:
  virtual ~UniversalTranslator() = default;

  // Connects to the permissioned binary frequency
  virtual bool connect(const std::string &credentials) = 0;

  // The Ingress Receptor callback
  virtual void on_market_data(const uint8_t *raw_photon, size_t len) = 0;

  // The Kinetic Egress command
  virtual void send_order(const uint8_t *precalculated_strike, size_t len) = 0;
};

// =========================================================================
// The Injection Point (Compile-Time Gating)
// =========================================================================

#ifdef HAS_ONIXS

// Authorized Injection. The proprietary SDK logic is mounted here.
// When compiled with `make -f Makefile.onixs`, the true Universal Translator
// maps to the direct-binary OnixS Solarflare Order Handler.
#include <onixs/solarflare_handler.h>
class OnixS_Proprietary_Graft : public UniversalTranslator {
  // Hidden proprietary implementation
};

inline UniversalTranslator *create_translator() {
  return new OnixS_Proprietary_Graft();
}

#else

// The Altruistic Stub. Maintains the build fidelity for the "Kid in India"
// running the high-performance chassis, without requiring the proprietary keys.
class NullTranslatorStub : public UniversalTranslator {
public:
  bool connect(const std::string &) override {
    std::cerr
        << "[Translator] Aether-Gated: OnixS SDK NOT injected. Sim-mode only."
        << std::endl;
    return false;
  }
  void on_market_data(const uint8_t *, size_t) override {}
  void send_order(const uint8_t *, size_t) override {}
};

inline UniversalTranslator *create_translator() {
  return new NullTranslatorStub();
}

#endif // HAS_ONIXS

} // namespace aether
} // namespace quanux

#endif // QUANUX_TRANSLATOR_STUB_HPP
