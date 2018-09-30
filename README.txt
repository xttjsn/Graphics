README for shape

This is the readme file for the shape project. In this project I created 6 shapes: cube, cone, cylinder, sphere, torus and a mobius strip.

To take advantage of polymorphism, I created a base class for all of them: the OpenGLShape class (which is a modified duplicate of the OpenGLShape class from lab1 and lab2). I also add implementations for VAO and VBO to make this class works as intended.

Additionally, I noticed that there are places that I could reuse code. For example, creating the bottom of the cylinder and the bottom of the cone is identical. So I created a utility class called ShapeUtil, which helps all the other shape classes to create their sub-geometry.

To create normals for each vertex, I specified such that each vertex data consists of 6 floats, where the first 3 are its position and the last 3 are its normal.

1. Cube
To create the cube, I firstly call the createQuadStrip() method provided by the ShapeUtil class, which creates a trip of small flat quads by interpolating the intermediate points from the four corners of the big quad. Then I duplicate that quad strip row by row to create a surface of the cube. Lastly I rotate that surface 5 more times for each and every face of the cube.

2. Cone
To create the bottom of the cone, I firtly call the createTriangleStrip() method provided by the ShapeUtil class, which interpolates the intermediate vertex from the three vertices of the big triangle. Then I rotate that triangle strip to form the bottom. To create the side of the cone, I use the same technique, only with different starting vertex (A) and ending vertices (B and C).

The normals of vertices on the side of the cone are pretty tricky to handle, since in createTriangleStrip() the normals created are the normal for the triangle face, but they are not the normal we are after. The correct normal for a vertex would be the average of two adjacent surface normals. So I use the interpolate() method to compute the averaged normals.

3. Cylinder
Creating the bottom of the cylinder is identical to cone. To create the sides of the cylinder, I use createQuadStrip() and then rotate it to get enough duplicates forming a circular form. I also handled the normals for each vertex as I did with cone.

4. Sphere
To compute the position of vertex in a sphere, I used the spherical coordinate formula, which uses the polar angle theta and the azimuth angle phi. The normals are very convenient to compute, since it's basically the normalized version of the coordinate.

5. Torus
I used a three-pass method to create the torus. I noticed that torus is formed by many sub-circles located in a circular form, with a fixed distance from the center of the torus. So I created two centers for two of those sub-circles (adjacent). Then I create two circles of vertices around those centers. Next, I connect those vertices to form a segment of the torus. Finally, I rotate and duplicate the segment to create the torus. The normals can be computed relative to the centers
of the sub-circles.

6. Mobius Strip
A mobius strip is a geometry with only one face. To create it, I used the Euclidean form of mobius strip (see my code), which takes input u and v, where u ranges from 0 to 2 * PI, and v ranges from -1 to 1. The normals are pretty tricky, since each vertex are shared by 6 triangles (except for the inner most and outer most vertices, which are only shared by 3 triangles), and the normals of those triangls are all different. Hence, for each normal, I compute the normals for all the triangles
needed, and take the average of them.

This only creates half of the mobius strip. To create the other half, I call the createMobiusStrip() function again, but with counter-clockwise order.


Known bugs:
No major bug, but I noticed that the lighting for one half of the mobius strip are kind of strange (dark-ish on some triangles).
