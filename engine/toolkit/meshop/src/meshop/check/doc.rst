The print inspection: a closed surface has ``watertight = 1``, ``holes = 0`` and
``non_manifold = 0``; ``genus`` counts handles. STL has no vertex sharing, so its importer yields
three vertices per triangle and ``genus`` for an STL read differs from the same model in OBJ or
PLY; the other checks still hold.
