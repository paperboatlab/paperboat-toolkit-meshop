# paperboat-toolkit-meshop

The optional triangle-mesh toolkit for [Paperboat](https://github.com/paperboatlab/paperboat): mesh
loading, exporting, topology statistics and printability checks on a `mesh` handle, built on
[VCGlib](https://github.com/cnr-isti-vclab/vcglib) without Qt or `meshlabserver`. Because VCGlib is
GPL-3.0, this toolkit is GPL-3.0, installed by the user and never bundled into Paperboat release
artifacts.

**Docs:** <https://paperboatlab.github.io/toolkits/meshop/> (sources in
[`engine/toolkit/meshop/docs/`](engine/toolkit/meshop/docs/) and each block's `doc.rst`, also in
the Help window (F1))

## Install

```sh
paperboat toolkit install meshop --repo https://github.com/paperboatlab/paperboat-toolkit-meshop.git
```

Declare `toolkits = ["meshop"]` in `backend.toml` and import `"meshop"` in `descriptor.toml`.

## Build

Against a Paperboat checkout or installed SDK (`-DCMAKE_PREFIX_PATH=<prefix>`):

```sh
cmake -S . -B build -G Ninja -DPAPERBOAT_SOURCE_DIR=../paperboat
cmake --build build
```

## Check

```sh
ctest --test-dir build --output-on-failure
```

## CI

[![ci](https://github.com/paperboatlab/paperboat-toolkit-meshop/actions/workflows/ci.yml/badge.svg?branch=dev)](https://github.com/paperboatlab/paperboat-toolkit-meshop/actions/workflows/ci.yml?query=branch%3Adev) builds the toolkit against `paperboat` (`dev`) and runs the mesh smoke tests.

## Licence

GNU General Public License version 3 (see [LICENSE](LICENSE)). VCGlib is GPL-3.0, so this toolkit
is too; it is never bundled into Paperboat packages (`pb#297`), and the Paperboat engine is
GPL-3.0-only as well.
