#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"

#include "glm/gtx/transform.hpp"
#include "qdebug.h"

Scene::Scene()
    : m_lod_coef(1)
{
}

Scene::Scene(Scene& scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
//    CS123SceneGlobalData global = { 1, 1, 1, 1 };
    setGlobal(scene.m_global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.

    m_transPrims = scene.m_transPrims;
    m_lights = scene.m_lights;
    m_lod_coef = scene.m_lod_coef;
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(),
    // addPrimitive(), and
    //   finishParsing()

    CS123SceneGlobalData gd;
    parser->getGlobalData(gd);
    sceneToFill->setGlobal(gd);

    CS123SceneLightData light;
    int numLights = parser->getNumLights();
    for (int i = 0; i < numLights; i++) {
        parser->getLightData(i, light);
        sceneToFill->addLight(light);
    }

    parseNode(sceneToFill, parser->getRootNode(), glm::mat4x4());

    sceneToFill->computeLOD();
}

void Scene::parseNode(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 parentTrans) {
    glm::mat4x4 curTransform = parentTrans;

    for (auto trans : node->transformations) {
        switch (trans->type) {
        case TRANSFORMATION_SCALE:
            curTransform *= glm::scale(trans->scale);
            break;

        case TRANSFORMATION_TRANSLATE:
            curTransform *= glm::translate(trans->translate);
            break;

        case TRANSFORMATION_ROTATE:
            curTransform *= glm::rotate(trans->angle, trans->rotate);
            break;

        case TRANSFORMATION_MATRIX:
            curTransform *= trans->matrix;
            break;

        default:
            qDebug("Invalid transformation type!");
            exit(-1);
        }
    }

    for (auto primitive : node->primitives) {
        sceneToFill->addPrimitive(*primitive, curTransform);
    }

    for (auto child : node->children) {
        parseNode(sceneToFill, child, curTransform);
    }
}

void Scene::addPrimitive(const CS123ScenePrimitive& scenePrimitive,
                         const glm::mat4x4        & matrix) {
    m_transPrims.emplace_back(matrix, scenePrimitive);
    m_transPrims[m_transPrims.size() - 1].primitive.material.cAmbient *= m_global.ka;
    m_transPrims[m_transPrims.size() - 1].primitive.material.cDiffuse *= m_global.kd;
    m_transPrims[m_transPrims.size() - 1].primitive.material.cSpecular *= m_global.ks;
    m_transPrims[m_transPrims.size() - 1].primitive.material.cTransparent *= m_global.kt;
}

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


void Scene::addLight(const CS123SceneLightData& sceneLight) {
    m_lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData& global)   {
    m_global = global;
}
