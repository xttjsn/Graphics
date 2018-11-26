#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"
#include "shapes/implicitcone.h"
#include "shapes/implicitcube.h"
#include "shapes/implicitcylinder.h"
#include "shapes/implicitsphere.h"
#include "shapes/implicittorus.h"
#include "SupportCanvas2D.h"
#include "threads/raytracethread.h"
#include <vector>
#include <map>
#include <QMutex>
#include <QImage>

const int MAX_RECURSION = 5;
const float RAY_OFFSET = 0.0001;
const float MIN_REFLECT = 0.01;

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
    void loadShapes();
    void loadMapData(CS123SceneMaterial& mat);
    void loadCameraMatrices(Camera* camera);
    void loadKDTree();
    void split(KDTreeNode* root);
    void trySplit(KDTreeNode* root, float& mincost, float& split, float& surfaceL, float& surfaceR, Axis axis);
    void rayTrace(float row, float col, int width, int height, BGRA& bgra);
    void rayTraceImpl(Ray& ray, BGRA& bgra, glm::vec4 reflectCoef, int recursionLevel);
    void kdTreeIntersect(KDTreeNode* root, Ray& ray, Intersect& intersect);
    void naiveIntersect(Ray& ray, Intersect& intersect);

    ImplicitShape* getShapePointer(PrimitiveType type);
    void returnShapePointer(PrimitiveType type, ImplicitShape* shape);
    glm::vec4 getFilmPixelPosition(float row, float col, int width, int height);
    glm::mat4x4 boundingBoxToTransform(BoundingBox& bbox);

    glm::vec4 calcNormal(Intersect& intersect);
    BGRA calcLight(Intersect& intersect, glm::vec4 normal);
    glm::vec4 getDiffuse(Intersect& intersect);

    std::unique_ptr<ImplicitCone> m_cone;
    std::unique_ptr<ImplicitCube> m_cube;
    std::unique_ptr<ImplicitCylinder> m_cylinder;
    std::unique_ptr<ImplicitSphere> m_sphere;
    std::unique_ptr<ImplicitTorus> m_torus;

    QMutex m_conemtx;
    QMutex m_cubemtx;
    QMutex m_cylindermtx;
    QMutex m_spheremtx;
    QMutex m_torusmtx;

    glm::mat4x4 m_view;
    glm::mat4x4 m_scale;
    glm::mat4x4 m_viewInv;
    glm::mat4x4 m_scaleInv;

    KDTreeNode    m_kd_root;

    RayTraceMaster *m_master;

    std::map<std::string, QImage> m_texture_images;

friend class RayTraceThread;
};

#endif // RAYSCENE_H
