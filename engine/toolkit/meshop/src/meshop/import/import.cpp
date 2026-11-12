// The import block: imports a triangle mesh (obj, ply, stl, off) from a file.
#include "meshop/meshop_shared.hpp"

namespace paperboat::toolkit::meshop {

void add_import(Registry& registry) {
  add_mesh_source(registry, "import", "Import mesh", "Imports a triangle mesh (obj, ply, stl, off) from a file.");
}

}  // namespace paperboat::toolkit::meshop
