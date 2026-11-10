// Meshes: reading, writing, the topology checks, and the scene entity.

#include "paperboat/toolkit/meshop.hpp"
#include "paperboat/toolkit/scene.hpp"
#include "vcg.hpp"  // vcg/complex/complex.h first: the wrap/ importers and exporters assume it

#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_off.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_stl.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_stl.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <stdexcept>
#include <string>

namespace paperboat::toolkit::meshop {
namespace {

enum class Format { Obj, Ply, Stl, Off };

std::string extension_of(const std::filesystem::path& file) {
  std::string ext = file.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return ext;
}

Format format_of(const std::filesystem::path& file) {
  const std::string ext = extension_of(file);
  if (ext == ".obj") return Format::Obj;
  if (ext == ".ply") return Format::Ply;
  if (ext == ".stl") return Format::Stl;
  if (ext == ".off") return Format::Off;
  throw std::runtime_error("meshop: \"" + file.string() + "\": expected .obj, .ply, .stl or .off");
}

template <class Importer>
Poly load_with(const std::string& path) {
  detail::Mesh mesh;
  int mask = 0;
  const int result = Importer::Open(mesh, path.c_str(), mask);
  if (result != 0)
    throw std::runtime_error("meshop: cannot read \"" + path + "\": " + Importer::ErrorMsg(result));
  return detail::from_vcg(mesh);
}

}  // namespace

Poly load(const std::filesystem::path& file) {
  const std::string path = file.string();
  switch (format_of(file)) {
    case Format::Obj: return load_with<vcg::tri::io::ImporterOBJ<detail::Mesh>>(path);
    case Format::Ply: return load_with<vcg::tri::io::ImporterPLY<detail::Mesh>>(path);
    case Format::Stl: return load_with<vcg::tri::io::ImporterSTL<detail::Mesh>>(path);
    case Format::Off: return load_with<vcg::tri::io::ImporterOFF<detail::Mesh>>(path);
  }
  throw std::runtime_error("meshop: unreachable format");
}

void save(const Poly& poly, const std::filesystem::path& file) {
  detail::Mesh mesh;
  detail::to_vcg(poly, mesh);
  const std::string path = file.string();
  if (const auto parent = file.parent_path(); !parent.empty()) {
    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
  }
  // mask 0: geometry only (positions and faces); binary for the formats that offer it.
  int result = 0;
  switch (format_of(file)) {
    case Format::Obj: result = vcg::tri::io::ExporterOBJ<detail::Mesh>::Save(mesh, path.c_str(), 0); break;
    case Format::Ply: result = vcg::tri::io::ExporterPLY<detail::Mesh>::Save(mesh, path.c_str(), 0, true); break;
    case Format::Stl: result = vcg::tri::io::ExporterSTL<detail::Mesh>::Save(mesh, path.c_str(), true, 0); break;
    case Format::Off: result = vcg::tri::io::ExporterOFF<detail::Mesh>::Save(mesh, path.c_str(), 0); break;
  }
  if (result != 0) throw std::runtime_error("meshop: cannot write \"" + path + "\"");
}

Check check(const Poly& poly) {
  Check out;
  if (poly.faces.empty()) return out;
  detail::Mesh mesh;
  detail::to_vcg(poly, mesh);
  vcg::tri::UpdateTopology<detail::Mesh>::FaceFace(mesh);
  out.watertight = vcg::tri::Clean<detail::Mesh>::IsWaterTight(mesh);
  out.holes = vcg::tri::Clean<detail::Mesh>::CountHoles(mesh);
  out.non_manifold_edges = vcg::tri::Clean<detail::Mesh>::CountNonManifoldEdgeFF(mesh);
  out.components = vcg::tri::Clean<detail::Mesh>::CountConnectedComponents(mesh);
  out.genus = vcg::tri::Clean<detail::Mesh>::MeshGenus(mesh);
  return out;
}

Entity mesh_entity(std::string id, const Poly& poly, const std::string& color, std::array<double, 3> at,
                   double scale) {
  std::vector<double> vertices;
  vertices.reserve(poly.positions.size() * 3);
  for (const auto& p : poly.positions) {
    vertices.push_back(p[0]);
    vertices.push_back(p[1]);
    vertices.push_back(p[2]);
  }
  std::vector<double> indices;
  indices.reserve(poly.faces.size() * 3);
  for (const auto& f : poly.faces) {
    indices.push_back(f[0]);
    indices.push_back(f[1]);
    indices.push_back(f[2]);
  }
  Entity e = scene::mesh(std::move(id), std::move(vertices), std::move(indices), color);
  e.pos = at;
  e.scale = {scale, scale, scale};
  return e;
}

}  // namespace paperboat::toolkit::meshop
