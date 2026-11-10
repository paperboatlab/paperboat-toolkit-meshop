// Smoke test for the meshop package: a unit cube round-trips through every format and the
// topology checks agree with what a cube is.

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "paperboat/toolkit/meshop.hpp"

using paperboat::toolkit::meshop::Poly;
using paperboat::toolkit::meshop::check;
using paperboat::toolkit::meshop::load;
using paperboat::toolkit::meshop::save;

namespace {

int failures = 0;

void expect(bool ok, const std::string& what) {
  std::printf("%s %s\n", ok ? "ok  " : "FAIL", what.c_str());
  if (!ok) ++failures;
}

Poly cube() {
  Poly p;
  p.positions = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
                 {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}};
  p.faces = {{0, 2, 1}, {0, 3, 2}, {4, 5, 6}, {4, 6, 7}, {0, 1, 5}, {0, 5, 4},
             {3, 7, 6}, {3, 6, 2}, {0, 4, 7}, {0, 7, 3}, {1, 2, 6}, {1, 6, 5}};
  return p;
}

double area_of(const Poly& p) {
  double area = 0.0;
  for (const auto& f : p.faces) {
    const auto& a = p.positions[f[0]];
    const auto& b = p.positions[f[1]];
    const auto& c = p.positions[f[2]];
    const double ux = b[0] - a[0], uy = b[1] - a[1], uz = b[2] - a[2];
    const double vx = c[0] - a[0], vy = c[1] - a[1], vz = c[2] - a[2];
    const double cx = uy * vz - uz * vy;
    const double cy = uz * vx - ux * vz;
    const double cz = ux * vy - uy * vx;
    area += 0.5 * std::sqrt(cx * cx + cy * cy + cz * cz);
  }
  return area;
}

}  // namespace

int main() {
  const Poly cube = ::cube();
  expect(std::abs(area_of(cube) - 6.0) < 1e-4, "cube area is 6");

  const auto c = check(cube);
  expect(c.watertight, "cube is watertight");
  expect(c.holes == 0, "cube has no holes");
  expect(c.genus == 0, "cube has genus 0");
  expect(c.non_manifold_edges == 0, "cube has no non-manifold edges");
  expect(c.components == 1, "cube has one component");

  const std::filesystem::path dir = std::filesystem::temp_directory_path() / "meshop-smoke";
  std::filesystem::create_directories(dir);
  for (const char* ext : {".obj", ".ply", ".stl", ".off"}) {
    const auto file = dir / ("cube" + std::string(ext));
    save(cube, file);
    const Poly back = load(file);
    // STL has no vertex sharing: its importer yields 3 vertices per triangle.
    const std::size_t least = std::string(ext) == ".stl" ? 36 : 8;
    expect(back.positions.size() >= least, std::string(ext) + ": vertices back");
    expect(back.faces.size() == 12, std::string(ext) + ": 12 faces back");
    expect(std::abs(area_of(back) - 6.0) < 1e-3, std::string(ext) + ": area survives");
  }

  Poly open = cube;
  open.faces.pop_back();
  expect(!check(open).watertight, "a cube with a face removed is not watertight");
  expect(check(open).holes == 1, "a cube with a face removed has one hole");

  std::printf("\n%s (%d failure(s))\n", failures == 0 ? "PASS" : "FAIL", failures);
  return failures == 0 ? 0 : 1;
}
