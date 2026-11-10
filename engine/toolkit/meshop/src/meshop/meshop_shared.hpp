#pragma once

// Shared by the load and save blocks.

#include <stdexcept>
#include <string>
#include <string_view>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {

// A block's file param (the descriptor's `type = "file"` reaches a block through Config).
inline std::string file_of(const Config& config, std::string_view what) {
  const auto it = config.find("file");
  if (it == config.end() || it->second.empty()) throw std::runtime_error(std::string(what) + " needs a file");
  return it->second;
}

}  // namespace paperboat::toolkit::meshop
