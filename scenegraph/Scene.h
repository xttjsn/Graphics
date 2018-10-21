#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {
    }

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);
    static void parseNode(Scene *sceneToFill, CS123SceneNode *node, glm::mat4x4 parentTrans);

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    std::vector<CS123TransfromPrimitive> m_transPrims;
    std::vector<CS123SceneLightData> m_lights;
    CS123SceneGlobalData m_global;
};

#endif // SCENE_H
