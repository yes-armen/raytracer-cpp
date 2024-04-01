# Raytracer P3

This is the third subtask of the [raytracer homework assignment](../raytracer). In this task, you need to implement simplified modes of the raytracer which will help you catch the main errors when working with the camera, ray tracing, and geometry implementation.

Read the description of the main task before performing this one. Here, you need to implement support for RenderMode::kDepth and RenderMode::kNormal. Note that the implementation must be done within the general raytracer task (i.e., you do not write a separate solution in this task) --- the testing system does not send files from this directory. However, to pass this task, support for RenderMode::kFull is not required.

In RenderMode::kDepth mode, when tracing a ray, the resulting pixel color will be the distance to the nearest object hit by the ray. Let's say that when emitting a ray from the camera through the next pixel, this ray hits an object at a distance $d$. Then, the resulting value for this pixel will be a triplet $(d, d, d)$. Note that eventually, all values must be brought into the range [0, 1]. To do this:

* Where the ray does not intersect the scene, use the value (1, 1, 1).
* Let the maximum distance across all objects in ray tracing be $D$. Then, the final value for the pixel will be a triplet $\left(\frac{d}{D}, \frac{d}{D}, \frac{d}{D}\right)$.

In RenderMode::kNormal mode, when tracing a ray, the resulting color will be the value of the normal at the point of intersection of the ray with the nearest object. The normal should be directed opposite to the direction of the ray. Since the normal vector is normalized, all coordinates will be in the range [-1, 1]. To display the image, they need to be brought into the range [0, 1]. To do this:

* Where the ray does not intersect the scene, use the value (0, 0, 0) as a result.
* Let's say the normal vector obtained is $\bar v$. Then use $\frac{1}{2}\bar v + \frac{1}{2}$ as a result.
* To submit the assignment, change the contents of this directory (for example, by creating an empty file)
and send these changes to your remote repository.
