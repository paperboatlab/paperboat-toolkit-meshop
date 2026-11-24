Install
=======

.. code-block:: sh

   paperboat toolkit install meshop --repo https://github.com/paperboatlab/simplicial-toolkit-meshop

A C++ station registers it next to the builtins:

.. code-block:: cpp

   tk::Registry r = tk::builtins();
   tk::meshop::register_meshop(r);

and imports ``"meshop"`` in its ``descriptor.toml``. A station that lists it in ``station.toml``
(``toolkits = ["meshop"]``) is checked for it by ``paperboat check``.

Building it yourself fetches what is not installed at configure time: VCGlib ``2025.07``
(header-only, plus its ``wrap/ply/plylib.cpp``) and Eigen ``5.0.1`` (header-only):

.. code-block:: sh

   # against a Paperboat checkout, or an installed SDK (-DCMAKE_PREFIX_PATH=<prefix>)
   cmake -S . -B build -G Ninja -DPAPERBOAT_SOURCE_DIR=../paperboat
   cmake --build build
   ctest --test-dir build --output-on-failure
