#include "StrategyLoader.hpp"
#include <dlfcn.h>
#include <iostream>

namespace QuanuX::ClearStreet {

StrategyLoader::StrategyLoader(const std::string &libPath)
    : libPath_(libPath), handle_(nullptr), instance_(nullptr),
      createFn_(nullptr), destroyFn_(nullptr) {}

StrategyLoader::~StrategyLoader() { unload(); }

void StrategyLoader::unload() {
  if (instance_ && destroyFn_) {
    destroyFn_(instance_);
  }
  instance_ = nullptr;

  if (handle_) {
    dlclose(handle_);
    handle_ = nullptr;
  }
}

bool StrategyLoader::load() {
  unload(); // Ensure clean state

  handle_ = dlopen(libPath_.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!handle_) {
    std::cerr << "DLOPEN Error: " << dlerror() << std::endl;
    return false;
  }

  // Load Symbols
  createFn_ = (CreateStrategyFn)dlsym(handle_, "createStrategy");
  destroyFn_ = (DestroyStrategyFn)dlsym(handle_, "destroyStrategy");

  if (!createFn_ || !destroyFn_) {
    std::cerr << "DLSYM Error: Missing factory functions" << std::endl;
    unload();
    return false;
  }

  // Create Instance
  instance_ = createFn_();
  return (instance_ != nullptr);
}

bool StrategyLoader::reload() {
  std::cout << "Reloading Strategy..." << std::endl;
  // In a real system, you might want to transfer state from old instance to
  // new. Here we just do a cold swap.
  return load();
}

IStrategy *StrategyLoader::getStrategy() const { return instance_; }

} // namespace QuanuX::ClearStreet
