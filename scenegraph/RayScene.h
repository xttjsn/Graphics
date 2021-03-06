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
const float RAY_OFFSET = 1e-3;
const float MIN_REFLECT = 0.001;
const int MAX_NUM_THREADS = 10;
const int SUB_SIZE = 50;
const float CENTER_WEIGHT = 0.5;
const float SUB_WEIGHT = 0.125;
const float MAX_VARIATION = 0.5;

/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    virtual ~RayScene();

    virtual void render(SupportCanvas2D* canvas, Camera* camera);

protected:

    void loadTextures();
    void loadShapes();
    void loadMapData(CS123SceneMaterial& mat);
    void loadCameraMatrices(Camera* camera);
    void loadKDTree();
    void split(KDTreeNode* root);
    void trySplit(KDTreeNode* root, float& mincost, float& split, Axis axis);
    glm::vec4 rayTrace(float row, float col);
    glm::vec4 superSampleRayTrace(float row, float col, int samplingLevel);
    glm::vec4 rayTraceImpl(Ray& ray, int recursionLevel);
    glm::vec4 superSampleRayTraceImpl(float row, float col);
    bool getRefractRay(const Ray& incident_ray, const Intersect& intersect, Ray& refract_ray, float ior);
    void kdTreeIntersect(KDTreeNode* root, Ray& ray, Intersect& intersect);
    void naiveIntersect(Ray& ray, Intersect& intersect);

    ImplicitShape* getShapePointer(PrimitiveType type);
    void returnShapePointer(PrimitiveType type, ImplicitShape* shape);
    glm::vec4 getFilmPixelPosition(float row, float col, int width, int height);
    glm::mat4x4 boundingBoxToTransform(BoundingBox& bbox);
    BGRA vec2bgra(glm::vec4 color);
    float colorVariation(glm::vec4 colorA, glm::vec4 colorB);

    glm::vec4 calcLight(const Ray& ray, const Intersect& intersect);
    glm::vec4 getDiffuse(const Intersect& intersect);

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

    std::unique_ptr<RayTraceMaster> m_master;

    std::map<std::string, QImage> m_texture_images;
    std::map<std::pair<float, float>, glm::vec4> m_color_map;

    SupportCanvas2D *m_canvas;


friend class RayTraceThread;
};

#endif // RAYSCENE_H
