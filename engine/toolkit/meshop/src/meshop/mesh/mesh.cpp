// The Mesh station object: a model on the Station, with its linked block.
#include <memory>
#include <string>
#include <vector>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {
namespace {

// A model on the Station: the file named by its `file` property, drawn `scale` units per mesh
// unit, in `color`. Its linked block puts the same mesh on the Flow page.
class MeshObject final : public StationObject {
 public:
  explicit MeshObject(const Values& props)
      : file_(get_text(props, "file")), color_(get_text(props, "color", "#888888")),
        scale_(get_or(props, "scale", 1.0)), poly_(load(file_)) {}

  Values read() const override { return {{"mesh", mesh_out(poly_)}}; }
  void step(const Context&, const Values&) override {}

  std::vector<Entity> entities(const std::string& id, double z) const override {
    return {mesh_entity(id, poly_, color_, {0.0, 0.0, z}, scale_)};
  }

 private:
  std::string file_;
  std::string color_;
  double scale_;
  Poly poly_;
};

}  // namespace

void add_mesh(Registry& registry) {
  registry.add(StationTemplate{
      .name = "meshop/mesh",
      .def = {.type = "mesh",
              .label = "Mesh",
              .role = "body",
              .props = {{.id = "file", .default_value = 0, .type = "file", .default_text = ""},
                        {.id = "color", .default_value = 0, .type = "text", .default_text = "#888888"},
                        {"scale", 1.0}},
              .links = {},
              .block = BlockDef{.type = "mesh",
                                .label = "Mesh",
                                .kind = Kind::Probe,
                                .inputs = {},
                                .outputs = {mesh_port("mesh")},
                                .params = {},
                                .description = "A triangle mesh from a file, for a flow to work on."},
              .description = "A triangle mesh from a file, for a flow to work on."},
      .make = [](const Values& props) { return std::make_unique<MeshObject>(props); },
  });
}

}  // namespace paperboat::toolkit::meshop
