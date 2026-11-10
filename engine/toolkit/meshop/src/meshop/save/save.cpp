// The save block: writes its mesh to a file, in the format of the extension.
#include <memory>
#include <string>

#include "meshop/meshop_shared.hpp"

namespace paperboat::toolkit::meshop {
namespace {

class Save final : public Block {
 public:
  explicit Save(const Config& config) : file_(file_of(config, "save")) {}

  Values step(const Context&, const Values& in) override {
    const Poly* mesh = mesh_in(in, "mesh");
    if (!mesh) return {};
    save(*mesh, file_);
    return {{"saved", 1.0}, {"path", file_}};
  }

 private:
  std::string file_;
};

}  // namespace

void add_save(Registry& registry) {
  Template t;
  t.name = "meshop/save";
  t.def = {.type = "save",
           .label = "Save mesh",
           .kind = Kind::Function,
           .inputs = {mesh_port("mesh")},
           .outputs = {{"saved"}, {.id = "path", .type = "text"}},
           .params = {{.id = "file", .type = "file", .default_text = ""}},
           .description = "Writes its mesh to a file (obj, ply, stl, off), by the extension.",
           .cacheable = false};
  t.make = [](const Values&, const Config& c) { return std::make_unique<Save>(c); };
  registry.add(std::move(t));
}

}  // namespace paperboat::toolkit::meshop
