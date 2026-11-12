// The meshop package: each block is a folder, meshop/<block>/<block>.cpp, defining add_<block>
// (mesh is the Mesh station object). This registers them.

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop {

void add_load(Registry&);
void add_import(Registry&);
void add_save(Registry&);
void add_stats(Registry&);
void add_check(Registry&);
void add_mesh(Registry&);

void register_meshop(Registry& registry) {
  add_load(registry);
  add_import(registry);
  add_save(registry);
  add_stats(registry);
  add_check(registry);
  add_mesh(registry);
}

}  // namespace paperboat::toolkit::meshop
