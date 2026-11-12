#pragma once

// Shared by the blocks: the file source (`load`, `import`) and `save`.

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {

// A block's file param (the descriptor's `type = "file"` reaches a block through Config).
inline std::string file_of(const Config& config, std::string_view what) {
  const auto it = config.find("file");
  if (it == config.end() || it->second.empty()) throw std::runtime_error(std::string(what) + " needs a file");
  return it->second;
}

// The source block `load` and `import` share: read the mesh at its `file` param when made and
// put it on `mesh`. `what` names the block in the error for a missing file. The imported mesh
// is also an entity on the Station, so the Board's 3D view can show it.
class MeshSource final : public Block {
 public:
  MeshSource(const Config& config, std::string_view what)
      : id_("meshop/" + std::string(what) + ":" + file_of(config, what)),
        poly_(load(file_of(config, what))),
        mesh_(mesh_out(poly_)) {}

  Values step(const Context&, const Values&) override { return {{"mesh", mesh_}}; }
  std::vector<Entity> entities() const override { return {mesh_entity(id_, poly_, "#888888", {0, 0, 0}, 1.0)}; }

 private:
  std::string id_;
  Poly poly_;
  Value mesh_;
};

// Register a source block `type` ("load", "import") that reads a mesh from a file.
inline void add_mesh_source(Registry& registry, std::string type, std::string label, std::string description) {
  const std::string what = type;  // the error names the block, e.g. "import needs a file"
  Template t;
  t.name = "meshop/" + type;
  t.def = {.type = std::move(type),
           .label = std::move(label),
           .kind = Kind::Source,
           .outputs = {mesh_port("mesh")},
           .params = {{.id = "file", .type = "file", .default_text = ""}},
           .description = std::move(description),
           .cacheable = false};
  t.make = [what](const Values&, const Config& c) { return std::make_unique<MeshSource>(c, what); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::meshop
