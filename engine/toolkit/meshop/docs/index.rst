meshop
======

The optional mesh toolkit for Paperboat: triangle mesh blocks on a ``mesh`` handle, built on
`VCGlib <https://github.com/cnr-isti-vclab/vcglib>`_, the algorithms behind MeshLab's filter menu,
without Qt, without the plugin ABI and without ``meshlabserver``.

.. toctree::
   :maxdepth: 1

   install
   blocks/index

Licence
-------

**GPL-3.0.** VCGlib is GPL-3.0, so this toolkit is too. It is installed by the user and **never
bundled**: it is not in the ``.deb``, ``.rpm``, ``.tar.gz``, ``.zip``, npm packages, Python wheel or
web bundle. The engine is GPL-3.0-only as well; this folder never enters an artifact.

Meshes on the wires
-------------------

A ``mesh`` wire carries plain arrays (float positions, uint32 triangles), so a backend can build one
without linking VCGlib; the handle kind is ``"mesh"``. The station object ``mesh`` draws a file in
the scene (``file``, ``color``, ``scale``) and its linked block gives the ``mesh``.

The blocks are pure and cacheable except ``load``, ``import`` and ``save``, which touch the
filesystem. ``import`` and ``load`` do the same work; ``load`` stays for existing descriptors.

Roadmap
-------

Still to come: ``clean`` (weld, drop degenerate, fill holes, orient), ``shape`` (decimate, remesh,
smooth, subdivide), ``exact`` (CGAL booleans, offsets, watertight wrap, segmentation, ARAP),
``print`` (hollow, drain, min wall, fit bed) and ``morph`` (``t`` on the clock).
