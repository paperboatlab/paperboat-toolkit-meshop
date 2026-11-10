// The load block: reads a triangle mesh (obj, ply, stl, off) from a file.
#include <memory>

#include "meshop/meshop_shared.hpp"

namespace paperboat::toolkit::meshop {
namespace {

class Load final : public Block {
 public:
  explicit Load(const Config& config) : mesh_(mesh_out(load(file_of(config, "load")))) {}

  Values step(const Context&, const Values&) override { return {{"mesh", mesh_}}; }

 private:
  Value mesh_;
};

}  // namespace

void add_load(Registry& registry) {
  Template t;
  t.name = "meshop/load";
  t.def = {.type = "load",
           .label = "Load mesh",
           .kind = Kind::Source,
           .outputs = {mesh_port("mesh")},
           .params = {{.id = "file", .type = "file", .default_text = ""}},
           .description = "Reads a triangle mesh (obj, ply, stl, off) from a file.",
           .cacheable = false};
  t.make = [](const Values&, const Config& c) { return std::make_unique<Load>(c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::meshop
