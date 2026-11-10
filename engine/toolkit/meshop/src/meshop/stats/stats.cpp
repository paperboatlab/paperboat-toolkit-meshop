// The stats block: counts, bounding box, surface area and volume of a mesh.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {
namespace {

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

}  // namespace

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

}  // namespace paperboat::toolkit::meshop
