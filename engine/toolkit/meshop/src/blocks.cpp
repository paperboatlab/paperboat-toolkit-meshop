// The meshop blocks: load, save, stats, check, and the Mesh station object.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {
namespace {

// A block's file param (the descriptor's `type = "file"` reaches a block through Config).
std::string file_of(const Config& config, std::string_view what) {
  const auto it = config.find("file");
  if (it == config.end() || it->second.empty()) throw std::runtime_error(std::string(what) + " needs a file");
  return it->second;
}

struct Vec3 {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 cross(const Vec3& a, const Vec3& b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
Vec3 at(const Poly& mesh, std::uint32_t index) {
  const auto& p = mesh.positions[index];
  return {p[0], p[1], p[2]};
}

struct Measures {
  double area = 0.0;
  double volume = 0.0;
  Vec3 min{};
  Vec3 max{};
};

Measures measure(const Poly& mesh) {
  Measures m;
  if (mesh.positions.empty()) return m;
  m.min = m.max = at(mesh, 0);
  for (std::size_t i = 1; i < mesh.positions.size(); ++i) {
    const Vec3 v = at(mesh, static_cast<std::uint32_t>(i));
    m.min = {std::min(m.min.x, v.x), std::min(m.min.y, v.y), std::min(m.min.z, v.z)};
    m.max = {std::max(m.max.x, v.x), std::max(m.max.y, v.y), std::max(m.max.z, v.z)};
  }
  for (const auto& face : mesh.faces) {
    const Vec3 a = at(mesh, face[0]);
    const Vec3 b = at(mesh, face[1]);
    const Vec3 c = at(mesh, face[2]);
    m.area += 0.5 * std::sqrt(dot(cross(b - a, c - a), cross(b - a, c - a)));
    m.volume += dot(a, cross(b, c)) / 6.0;
  }
  m.volume = std::abs(m.volume);
  return m;
}

class Load final : public Block {
 public:
  explicit Load(const Config& config) : mesh_(mesh_out(load(file_of(config, "load")))) {}

  Values step(const Context&, const Values&) override { return {{"mesh", mesh_}}; }

 private:
  Value mesh_;
};

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

class Stats final : public Block {
 public:
  Values step(const Context&, const Values& in) override {
    const Poly* mesh = mesh_in(in, "mesh");
    if (!mesh) return {};
    const Measures m = measure(*mesh);
    return {{"vertices", static_cast<double>(mesh->positions.size())},
            {"faces", static_cast<double>(mesh->faces.size())},
            {"area", m.area},
            {"volume", m.volume},
            {"dx", m.max.x - m.min.x},
            {"dy", m.max.y - m.min.y},
            {"dz", m.max.z - m.min.z},
            {"cx", (m.min.x + m.max.x) / 2.0},
            {"cy", (m.min.y + m.max.y) / 2.0},
            {"cz", (m.min.z + m.max.z) / 2.0}};
  }
};

class CheckBlock final : public Block {
 public:
  Values step(const Context&, const Values& in) override {
    const Poly* mesh = mesh_in(in, "mesh");
    if (!mesh) return {};
    const Check c = check(*mesh);
    return {{"watertight", c.watertight ? 1.0 : 0.0},
            {"holes", static_cast<double>(c.holes)},
            {"genus", static_cast<double>(c.genus)},
            {"non_manifold", static_cast<double>(c.non_manifold_edges)},
            {"components", static_cast<double>(c.components)}};
  }
};

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

void add_stats(Registry& registry) {
  Template t;
  t.name = "meshop/stats";
  t.def = {.type = "stats",
           .label = "Mesh stats",
           .kind = Kind::Function,
           .inputs = {mesh_port("mesh")},
           .outputs = {{"vertices"}, {"faces"}, {"area"}, {"volume"}, {"dx"}, {"dy"}, {"dz"},
                       {"cx"}, {"cy"}, {"cz"}},
           .params = {},
           .description = "Counts, bounding box, surface area and volume of a mesh.",
           .cacheable = true};
  t.make = [](const Values&, const Config&) { return std::make_unique<Stats>(); };
  registry.add(std::move(t));
}

void add_check(Registry& registry) {
  Template t;
  t.name = "meshop/check";
  t.def = {.type = "check",
           .label = "Check mesh",
           .kind = Kind::Function,
           .inputs = {mesh_port("mesh")},
           .outputs = {{"watertight"}, {"holes"}, {"genus"}, {"non_manifold"}, {"components"}},
           .params = {},
           .description = "Topology for print: watertight, holes, genus, non-manifold edges, components.",
           .cacheable = true};
  t.make = [](const Values&, const Config&) { return std::make_unique<CheckBlock>(); };
  registry.add(std::move(t));
}

void add_mesh_object(Registry& registry) {
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

}  // namespace

void register_meshop(Registry& registry) {
  add_load(registry);
  add_save(registry);
  add_stats(registry);
  add_check(registry);
  add_mesh_object(registry);
}

}  // namespace paperboat::toolkit::meshop
