# paperboat-toolkit-meshop

The optional mesh toolkit for [Paperboat](https://github.com/paperboatlab/paperboat): triangle mesh
blocks on a `mesh` handle, built on [VCGlib](https://github.com/cnr-isti-vclab/vcglib) — the
algorithms behind MeshLab's filter menu, without Qt, without the plugin ABI, and without
`meshlabserver`.

**Licence: GPL-3.0.** VCGlib is GPL-3.0, so this toolkit is too. It is installed by the user and
**never bundled**: it is not in the `.deb`, `.rpm`, `.tar.gz`, `.zip`, npm packages, Python wheel or
web bundle (pb#297). The engine stays Apache-2.0 because this folder never enters an artifact.

**Docs:** <https://paperboatlab.github.io/toolkits/meshop/>, also in the Help window (F1) of
Paperboat. Their sources live with the code: `engine/toolkit/meshop/docs/` for the toolkit's pages,
and a block's notes in its folder (`src/meshop/<block>/doc.rst`); a block's ports and params come
from the catalog, so write only what the catalog can't say.

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

## Test

`engine/toolkit/meshop/tests/smoke.cpp` builds a unit cube, checks its area, topology and genus,
round-trips it through obj/ply/stl/off, removes a face to confirm that `check` sees the hole, and
runs the `import` block over a tiny OBJ (a missing file fails).
