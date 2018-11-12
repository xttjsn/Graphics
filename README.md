## README file for intersect

In this assignment I created new classes to handle the intersection problem.
I first create the "ImplicitShape" class, who acts as the parent class for specific sub-classes like "ImplictCube", "ImplicitCone" etc. 

I also created several classes to help move data around:
- Ray
- Intersect
- BoundingBox
- KDTreeNode
- KDTreePrimitive

In Ray I simply store the start and the delta vector. They are both in world space. In Intersect I store the position of the intersection (world space), the value of t (since the ray is unique in a local scope, amiguity is not an issue here), and a pointer to the CS123TransformPrimitive, which has the model transform matrix, the type of the primitive as well as the material of the primitive.

I maintained the flyweight pattern by storing 5 such unique_ptrs in the RayScene class. 

When doing intersection, I made sure that the value of t must be greater than 0 because negative values represent objects at the back of the camera.

I build the KDTree based on the weighted cost algorithm shown in the lecture, and I delete primitives for all nodes but the leaf nodes to save space. Although I sorted the primitives based on their xMin, yMin and zMin in each recursion, it proves to be not as bad as it sounds, since the number of primitives decreases exponentially on average, and it's a one-time computation for a scene.

For multi-threading, I used the QThread class provided by QT. Each thread is responsible for doing ray-tracing for a small square area of the screen. 

Currently all threads are started concurrently, but it might causes excessive context switching for large canvas. An improvement would be to run a fixed number of threads at any given time so as to minimize this effect.

Known Bugs:
I haven't implemented the torus and supersampling.




