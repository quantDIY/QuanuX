#include "QuickFIXFactory.hpp"
#include "QuickFIXBackend.hpp"

namespace QuanuX {
namespace ClearStreet {

std::unique_ptr<IEngine> createQuickFIXBackend() {
  return std::unique_ptr<IEngine>(new QuickFIXBackend());
}

} // namespace ClearStreet
} // namespace QuanuX
