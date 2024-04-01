# Raytracer P1

This is the first subtask of the [raytracer homework assignment](../raytracer). In this task, you need to implement all the necessary geometric primitives required in the raytracer.

First, finish the definitions of the main objects in vector.h, ray.h, sphere.h, and triangle.h. The key object here is the 3d-vector, note that it uses std:array for storage.
Creating a std::array is almost free compared to std::vector, and since many vectors need to be created in a raytracer, it is important to use this container to avoid significant performance degradation.
You may also define any other functions and operators for your convenience in implementing the raytracer, for example, it makes sense to define additional arithmetic operators for working with vectors.

After that, implement the following functions in geometry.h:

* GetIntersection, which finds the intersection of a ray with a sphere or a ray with a triangle. Assume that the triangle is non-degenerate.
* Refract(ray, normal, eta), which calculates the refracted ray relative to ray (if any). Here normal is the normal at the point where the ray transitions into another medium, and eta equals $\frac{n_1}{n_2}$, where $n_1, n_2$ are the refractive indices of the medium from which the ray comes and into which it goes, respectively.
* Reflect(ray, normal), which calculates the ray reflected from ray at a point where the surface normal equals normal. In both functions, assume that the normal vector is normalized and oriented opposite to the direction of ray.
* GetBarycentricCoords, calculating the barycentric coordinates of a point relative to a given triangle (it is guaranteed that the point lies inside or on the boundary of the triangle).

## Useful materials

* Ray-sphere intersection is described here http://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html
* Ray-triangle intersection can be done according to this algorithm: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
* Formulas for calculating reflected and refracted rays are given here: https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
* Formula for calculating barycentric coordinates: https://ru.wikipedia.org/wiki/%D0%91%D0%B0%D1%80%D0%B8%D1%86%D0%B5%D0%BD%D1%82%D1%80%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B5_%D0%BA%D0%BE%D0%BE%D1%80%D0%B4%D0%B8%D0%BD%D0%B0%D1%82%D1%8B
