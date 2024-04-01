# Raytracer P2

This is the second subtask of the [raytracer homework assignment](../raytracer). In this task, you need to implement a reader for .obj files, which define the scene for the raytracer.

The format description can be viewed on Wikipedia: https://en.wikipedia.org/wiki/Wavefront_.obj_file. In our case, the following points can be noted:

* Only lines with v, vt, vn, f, mtllib, and usemtl need to be supported; everything else should be ignored.
* The numbering of entities in f is global, i.e., various grouping modifiers like g or o should be ignored.
* f can specify an arbitrary number of vertices, but you can immediately slice such a polygon into triangles $(P_0, P_i, P_{i+1})$, where $i$ runs through the vertices. It is necessary to support all possible ways of specifying vertices in f (with the index of the normal and/or texture). However, there's no need to implement texture support itself.
* It is guaranteed that the file defined in mtllib is located in the same directory as the .obj file.
* For convenience, in our task, it's also necessary to handle lines of the form S x y z r. Such a line defines a sphere with a center at (x, y, z) and radius r.
* Additionally, it's required to process lines of the form P x y z r g b. This line defines a point light source with coordinates (x, y, z), having an intensity of (r, g, b). Note that (r, g, b) do not necessarily lie within the range [0,1].

Regarding .mtl files, the description is provided on the same Wikipedia page, with the following nuances:

* Lines with newmtl, Ka (the ambient_color field in Material), Kd (diffuse_color), Ks (specular_color), Ke (intensity), Ns (specular_exponent), Ni (refraction_index) need to be supported. Everything else should be ignored.
* The Ke modifier is non-standard but often found in real .mtl files. In our task, it should be treated the same as Ka, i.e., as an additive addition to the total illumination of the object.
* In our task, it's necessary to support the following additional modifier al a b c (albedo), defining the material's light-reflecting characteristics. How they are accounted for in the lighting calculation is described in the main task. If the modifier is not specified, then by default the values should be al 1 0 0.

All necessary functions for implementation are provided in scene.h. Getter functions should return objects in the order they are defined in the file. In Read and GetMaterials, the map key is the material name.
