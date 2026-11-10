#pragma once

// The meshop package: triangle mesh operations as blocks, meshes on the wires.
//
// - Flow: `load` puts a mesh (obj, ply, stl, off) on the wire; `stats` reports counts, size,
//   area and volume; `check` reports watertightness, holes, genus and non-manifold edges;
//   `save` writes it back.
// - Station: a Mesh object (a file, set on the Station page). Its linked block puts the mesh on
//   the Flow page, and the scene draws it through scene::mesh.
// - Your own blocks take a mesh in with mesh_in() and send one out with mesh_out(), like the
//   examples' inspectors.
//
// Optional, and GPL-3.0: it links VCGlib. Never bundled (pb#297): install it with
// `paperboat toolkit install meshop`, and a backend that lists it in backend.toml is checked by
// `paperboat check`.
//
//   tk::Registry r = tk::builtins();
//   tk::meshop::register_meshop(r);

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "paperboat/toolkit/registry.hpp"

namespace paperboat::toolkit::meshop {

inline constexpr std::string_view kMesh = "mesh";  // the handle kind and port type

// An indexed triangle mesh: x, y, z per vertex and three vertex indices per face, in the mesh's
// own frame. Positions are float (what VCGlib stores and what the wire carries).
struct Poly {
  std::vector<std::array<float, 3>> positions = {};
  std::vector<std::array<std::uint32_t, 3>> faces = {};
};

// The mesh at input `port`, or null if it's unwired or not a mesh.
inline const Poly* mesh_in(const Values& in, std::string_view port) { return get_handle<Poly>(in, port, kMesh); }
inline Value mesh_out(Poly mesh) { return make_handle(std::string(kMesh), std::move(mesh)); }
inline PortDef mesh_port(std::string id) { return {std::move(id), std::string(kMesh)}; }

Poly load(const std::filesystem::path& file);                 // by the file's extension; throws if it can't be read
void save(const Poly& mesh, const std::filesystem::path& file);  // by the file's extension; throws on failure

// What `check` reports, from VCGlib's topology: a watertight closed surface has no holes and no
// non-manifold edges, and its genus is the number of handles.
struct Check {
  bool watertight = false;
  int holes = 0;
  int genus = 0;
  int non_manifold_edges = 0;
  int components = 1;
};
Check check(const Poly& mesh);

// The mesh as a scene entity, `scale` units per mesh unit, centred on (x, y, z).
Entity mesh_entity(std::string id, const Poly& mesh, const std::string& color, std::array<double, 3> at,
                   double scale);

void register_meshop(Registry& registry);

}  // namespace paperboat::toolkit::meshop
