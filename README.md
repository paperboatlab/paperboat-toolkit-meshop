# paperboat-toolkit-meshop

The optional mesh toolkit for [Paperboat](https://github.com/paperboatlab/paperboat): triangle mesh
blocks on a `mesh` handle, built on [VCGlib](https://github.com/cnr-isti-vclab/vcglib) — the
algorithms behind MeshLab's filter menu, without Qt, without the plugin ABI, and without
`meshlabserver`.

**Licence: GPL-3.0.** VCGlib is GPL-3.0, so this toolkit is too. It is installed by the user and
**never bundled**: it is not in the `.deb`, `.rpm`, `.tar.gz`, `.zip`, npm packages, Python wheel or
web bundle (pb#297). The engine stays Apache-2.0 because this folder never enters an artifact.

## What it does now

| Block | In → out | Params |
| --- | --- | --- |
| `load` | → `mesh` | `file` (obj, ply, stl, off) |
| `import` | → `mesh` | `file` (obj, ply, stl, off) |
| `save` | `mesh` → `saved`, `path` | `file` (by extension) |
| `stats` | `mesh` → `vertices`, `faces`, `area`, `volume`, `dx`, `dy`, `dz`, `cx`, `cy`, `cz` | — |
| `check` | `mesh` → `watertight`, `holes`, `genus`, `non_manifold`, `components` | — |
| station object `mesh` | a file, drawn in the scene, linked block gives `mesh` | `file`, `color`, `scale` |

`check` is the print inspection: a closed surface has `watertight = 1`, `holes = 0` and
`non_manifold = 0`; `genus` counts handles.

## Install

Overlay this repository onto a Paperboat checkout so the toolkit sits at `engine/toolkit/meshop/`
(the engine discovers it from its `toolkit.toml`), or use the installer:

```sh
paperboat toolkit install meshop --repo https://github.com/paperboatlab/paperboat-toolkit-meshop
paperboat backends
```

Then in your backend's `main`:

```cpp
tk::Registry r = tk::builtins();
tk::meshop::register_meshop(r);
```

and import `"meshop"` in `descriptor.toml`. A backend that lists it in `backend.toml`
(`toolkits = ["meshop"]`) is checked for it by `paperboat check`.

## Build

```sh
# against a Paperboat checkout, or a installed SDK (`-DCMAKE_PREFIX_PATH=<prefix>`)
cmake -S . -B build -G Ninja -DPAPERBOAT_SOURCE_DIR=../paperboat
cmake --build build
ctest --test-dir build --output-on-failure
```

Dependencies are fetched at configure time when they are not installed: VCGlib `2025.07`
(header-only, plus its `wrap/ply/plylib.cpp`) and Eigen `5.0.1` (header-only). VCGlib's `wrap/`
importers need `vcg/complex/complex.h` included first; `src/vcg.hpp` does that.

## Notes

- A `mesh` wire carries plain arrays (`meshop::Poly`: float positions, uint32 triangles). A backend
  can build one without linking VCGlib; the handle kind is `"mesh"`.
- STL has no vertex sharing: its importer yields three vertices per triangle, so `vertices` and
  `genus` for an STL read differ from the same model in OBJ/PLY. `check` still works.
- `stats` computes area and volume from the triangles (`volume` is the absolute divergence sum), so
  it needs no normals or topology.
- The blocks are pure and cacheable except `load`, `import` and `save`, which touch the filesystem.
  `import` and `load` do the same work; `load` stays for existing descriptors.

## Roadmap

The rest of the epic (pb#296): `clean` (weld, drop degenerate, fill holes, orient), `shape`
(decimate, remesh, smooth, subdivide), `exact` (CGAL booleans, offsets, watertight wrap,
segmentation, ARAP), `print` (hollow, drain, min wall, fit bed) and `morph` (`t` on the clock).

## Test

`engine/toolkit/meshop/tests/smoke.cpp` builds a unit cube, checks its area, topology and genus,
round-trips it through obj/ply/stl/off, removes a face to confirm that `check` sees the hole, and
runs the `import` block over a tiny OBJ (a missing file fails).
