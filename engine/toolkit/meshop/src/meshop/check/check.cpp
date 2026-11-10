// The check block: topology for print (watertight, holes, genus, non-manifold edges, components).
#include <memory>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {
namespace {

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

}  // namespace

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

}  // namespace paperboat::toolkit::meshop
