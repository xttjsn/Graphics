#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"
#include "shapes/implicitcone.h"
#include "shapes/implicitcube.h"
#include "shapes/implicitcylinder.h"
#include "shapes/implicitsphere.h"
#include "shapes/implicittorus.h"
#include "SupportCanvas2D.h"
#include <vector>

struct tsIntersectComp {
    inline bool operator() (Intersect a, Intersect b) {
        return a.t < b.t;
    }
};

/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    virtual ~RayScene();

    virtual void render(SupportCanvas2D* canvas, Camera* camera, int width, int height);

protected:

    void loadTextures();
    void loadMapData(CS123SceneMaterial& mat);
    void loadKDTree();
    void split(KDTreeNode* root);
    void trySplit(KDTreeNode* root, float& mincost, float& split, float& surfaceL, float& surfaceR, Axis axis);
    void rayTrace(Camera* camera, int row, int col, int width, int height, BGRA& bgra);
    void kdTreeIntersect(KDTreeNode* root, Ray& ray, Intersect& intersect);

    ImplicitShape* getShapePointer(PrimitiveType type);
    void returnShapePointer(PrimitiveType type, ImplicitShape* shape);
    glm::vec4 getFilmPixelPosition(int row, int col, int width, int height);
    glm::mat4x4 boundingBoxToTransform(BoundingBox& bbox);

    std::unique_ptr<ImplicitCone> m_cone;
    std::unique_ptr<ImplicitCube> m_cube;
    std::unique_ptr<ImplicitCylinder> m_cylinder;
    std::unique_ptr<ImplicitSphere> m_sphere;
    std::unique_ptr<ImplicitTorus> m_torus;

    KDTreeNode    m_kd_root;
};

#endif // RAYSCENE_H
