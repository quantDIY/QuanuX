#pragma once

#include "../IEngine.hpp"
#include <memory>

namespace QuanuX {
namespace ClearStreet {

/**
 * @brief Factory to create QuickFIX backend.
 *
 * This is separated into a factory because QuickFIX headers contain
 * dynamic exception specifications (throw(...)) which are removed in C++20.
 * The implementation file for this factory is compiled with C++14.
 */
std::unique_ptr<IEngine> createQuickFIXBackend();

} // namespace ClearStreet
} // namespace QuanuX
