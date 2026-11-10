#pragma once

// The VCGlib side of meshop: the mesh type behind a `mesh` handle, its conversions, and the
// topology checks. VCGlib wants its own vertex/face component lists; the wire type (Poly) stays
// plain arrays so a backend can build one without linking VCGlib. (The namespace is `detail`, not
// `vcg`: inside `meshop`, a nested `vcg` would shadow the library's own namespace.)

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/complex.h>

#include "paperboat/toolkit/meshop.hpp"

namespace paperboat::toolkit::meshop::detail {

struct Types;
struct Vertex;
struct Face;

// FFAdj: the filter plugins' adjacency, required by the topology checks (holes, genus, components).
struct Types : vcg::UsedTypes<vcg::Use<Vertex>::AsVertexType, vcg::Use<Face>::AsFaceType> {};
struct Vertex : vcg::Vertex<Types, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags> {};
struct Face : vcg::Face<Types, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::FFAdj, vcg::face::BitFlags> {};
struct Mesh : vcg::tri::TriMesh<std::vector<Vertex>, std::vector<Face>> {};

// Fills `mesh` in place: vcg::tri::TriMesh is neither copyable nor movable.
inline void to_vcg(const Poly& poly, Mesh& mesh) {
  for (const auto& face : poly.faces) {
    for (const std::uint32_t index : face) {
      if (index >= poly.positions.size())
        throw std::invalid_argument("meshop: a face uses vertex " + std::to_string(index) + " of " +
                                    std::to_string(poly.positions.size()));
    }
  }
  auto vertices = vcg::tri::Allocator<Mesh>::AddVertices(mesh, poly.positions.size());
  for (std::size_t i = 0; i < poly.positions.size(); ++i) {
    const auto& p = poly.positions[i];
    vertices[i].P() = vcg::Point3f(p[0], p[1], p[2]);
  }
  auto faces = vcg::tri::Allocator<Mesh>::AddFaces(mesh, poly.faces.size());
  for (std::size_t i = 0; i < poly.faces.size(); ++i) {
    faces[i].V(0) = &mesh.vert[poly.faces[i][0]];
    faces[i].V(1) = &mesh.vert[poly.faces[i][1]];
    faces[i].V(2) = &mesh.vert[poly.faces[i][2]];
  }
}

inline Poly from_vcg(Mesh& mesh) {
  Poly poly;
  poly.positions.reserve(mesh.vn);
  for (auto& vertex : mesh.vert) {
    if (vertex.IsD()) continue;
    poly.positions.push_back({vertex.P()[0], vertex.P()[1], vertex.P()[2]});
  }
  poly.faces.reserve(mesh.fn);
  for (auto& face : mesh.face) {
    if (face.IsD()) continue;
    poly.faces.push_back({static_cast<std::uint32_t>(vcg::tri::Index(mesh, face.V(0))),
                          static_cast<std::uint32_t>(vcg::tri::Index(mesh, face.V(1))),
                          static_cast<std::uint32_t>(vcg::tri::Index(mesh, face.V(2)))});
  }
  return poly;
}

}  // namespace paperboat::toolkit::meshop::detail
