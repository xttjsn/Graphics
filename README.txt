README file for Ray

In this assignment I implemented the following features:

    - Super sampling (non-adpative)
    - Shadows
    - Texture mapping
    - Reflection
    - Multithreading
    - Use KD Tree

    - Point lights
    - Directional lights


I reused the classes from intersect, e.g. Intersect, Ray.

To build the reflection mechanism, I separate the function rayTraceImpl, which now accpets an additional parameter `recursionLevel`. Within the function I check whether the recursion level exceeds the maximum recursion level. If so, I stop the recursion, and if not the recursion continues. The color computed from sub-recursion will be added to the color of the current level, and multiplied by the reflection coefficients of the material of the current intersection.

I also adjusted the calcLight() function such that the "eye" point is binded to the start position of the ray. In addition, to account for texture map, I add a method for each implicit shape called "getUV". Within getDiffuse(), I mixed the original diffuse color with the texture color to the the final color.

I also fixed bugs of split() (the function that constructs KD-tree) where the surface calculation is incorrect when there are completly overlapped objects.

I also fixed bugs in ImplicitCylinder::intersect() and ImplicitCube::surfaceArea()

I put all changeble constant in RayScene.h (e.g. MAX_RECURSION, MAX_NUM_THREADS).

Known bugs:
    No known bugs
