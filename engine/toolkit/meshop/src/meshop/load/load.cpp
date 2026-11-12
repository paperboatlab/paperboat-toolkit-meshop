// The load block: reads a triangle mesh (obj, ply, stl, off) from a file.
#include "meshop/meshop_shared.hpp"

namespace paperboat::toolkit::meshop {

void add_load(Registry& registry) {
  add_mesh_source(registry, "load", "Load mesh", "Reads a triangle mesh (obj, ply, stl, off) from a file.");
}

}  // namespace paperboat::toolkit::meshop
