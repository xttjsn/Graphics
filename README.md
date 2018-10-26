## Sceneview

### Basic requirements
In this assignment I implemented the logic of building the shapes by using the data provided by the
`CS123XMLSceneParser`.

As described in the algo, I created a class in `CS123SceneData.h` called `CS123TransfromPrimitive`,
which has the following three member variables.

```
    glm::mat4x4 transform;
    CS123ScenePrimitive primitive;
```

The "transform" matrix is the "aggregated" transform matrix of the primitive, which is the product
of all its parent nodes' transforms as well as its own transforms, multiplied in a top first
order. We store such a matrix for each primitive in the scene so that we don't have to re-traverse
the tree again in the future. We only create such an object for primitives because those are what
ultimately gets drawn. Nodes' like "master" are only relevant because they apply transforms to
their children nodes. Once we've computed all the "aggregated" transform matrix for all the leaf
node (i.e. primitives), we no longer need their parents.

So, each `Scene` contains a `std::vector<CS123TransformPrimitive>` called `transPrim`, and we build this
via calling the function `parseNode` recursively in `Scene.cpp`. This function accepts a Scene
pointer, a node, and a transform matrix. It will first compute the local transform matrix and then
the product of the "inherited" transform matrix with it. If this node is primitive, it will add a
primitive to `transPrim`. Then for every child it has, we call `parseNode` again.

To avoid repeated vertices computation, we store the following 5 primitives in the `SceneviewScene`
class:

```
    std::unique_ptr<Cube> m_cube;
    std::unique_ptr<Cone> m_cone;
    std::unique_ptr<Sphere> m_sphere;
    std::unique_ptr<Cylinder> m_cylinder;
    std::unique_ptr<Torus> m_torus;
```

During geometry rendering, we determine which primitives to use depending on the primitive's type.
We only reset those shapes when settings are changed. Thus regardless of the complexity of the scene,
we never need to compute vertices for more than 5 primitives.

### Extra Credits

#### Texture Map

To implement texture map, we must store extra 2 floating point values in each vertex. Thus I created
a new class `OpenGLVertex` to hold all the data that a vertex needs.

```
class OpenGLVertex {
public:
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 texcoord;

    OpenGLVertex(glm::vec4 position, glm::vec4 normal, glm::vec2 texcoord);
    OpenGLVertex rotate(glm::mat4x4 rot);
    OpenGLVertex sphericalRotate(glm::mat4x4 rot);
    OpenGLVertex normAvgWithRotation(glm::mat4x4 rot);
};
```

I also indicates that there are 2 more floating point values for each vertex when populating the
vertex data to OpenGL.

```
static constexpr int kFloatsPerVertex = 8;
m_numVertices = m_coords.size() / kFloatsPerVertex;
setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
```

For efficient use of textures, I used the class `Texture2D` provided by the stencil code, and built
a hashmap from the filename of the texture to the `Texture2D` object. That way when multiple
primitives shares one texture, we can reuse the texture and save both space and I/O time.

```
std::map<std::string, CS123::GL::Texture2D> m_textures;
```

The process of building UVs varies from shape to shape. In a high-level, I replaced all the old
`ShapeUtil` function calls with a version that also computes the UV for each vertex. The methods
ranges from

- `sphericalUV` (from [the formula](https://en.wikipedia.org/wiki/UV_mapping))
- `rectangleUV`  ![](https://upload.wikimedia.org/wikipedia/commons/thumb/0/04/UVMapping.png/400px-UVMapping.png)
- `circularUV`  ![](circularUV.png)

#### Level of Details (LOD)

My implementation of LOD only depends on the number of primitives in the scene. I used the following
code to compute LOD.

```
void Scene::computeLOD() {
    int sz = m_transPrims.size();
    if (sz < 10)
        m_lod_coef = 1;
    else if (sz < 100)
        m_lod_coef = 0.75;
    else if (sz < 200)
        m_lod_coef = 0.5;
    else if (sz < 500)
        m_lod_coef = 0.25;
    else m_lod_coef = 0.1;
}
```

Thus `m_lod_coef` is always less than or equal to 1 and greater than or equal to 0. I then
multiplied `m_lod_coef` with `p1`, `p2` and `p3` from `settings` when setting up the 5 primitives.

### Bugs:

No known bugs
