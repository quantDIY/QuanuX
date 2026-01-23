#pragma once
#include <dlfcn.h>
#include <iostream>
#include <stdexcept>
#include <string>

class PluginLoader {
public:
  explicit PluginLoader(const std::string &path) {
    handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle_) {
      std::string err = dlerror();
      throw std::runtime_error("Failed to load plugin '" + path + "': " + err);
    }
  }

  ~PluginLoader() {
    if (handle_) {
      dlclose(handle_);
    }
  }

  template <typename T> T get_symbol(const std::string &symbol_name) {
    // Clear any existing error
    dlerror();
    void *sym = dlsym(handle_, symbol_name.c_str());
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      throw std::runtime_error("Failed to load symbol '" + symbol_name +
                               "': " + dlsym_error);
    }
    return reinterpret_cast<T>(sym);
  }

  // Disable copy
  PluginLoader(const PluginLoader &) = delete;
  PluginLoader &operator=(const PluginLoader &) = delete;

  // Allow move
  PluginLoader(PluginLoader &&other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }
  PluginLoader &operator=(PluginLoader &&other) noexcept {
    if (this != &other) {
      if (handle_)
        dlclose(handle_);
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

private:
  void *handle_ = nullptr;
};
