#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include "gl/textures/TextureParametersBuilder.h"

#include "shapes/cube.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "shapes/torus.h"
using namespace CS123::GL;


SceneviewScene::SceneviewScene() :
    m_initialized(false)
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene
    // here...
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/default.frag");

    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/wireframe.frag");

    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normals.frag");

    m_normalsShader = std::make_unique<Shader>(vertexSource,
                                               geometrySource,
                                               fragmentSource);
}

void SceneviewScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(
                ":/shaders/normalsArrow.frag");

    m_normalsArrowShader = std::make_unique<Shader>(vertexSource,
                                                    geometrySource,
                                                    fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights();
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();
}

void SceneviewScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();

    m_phongShader->setUniform("useLighting",  settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p",              camera->getProjectionMatrix());
    m_phongShader->setUniform("v",              camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void SceneviewScene::setLights()
{
    // TODO: [SCENEVIEW] Fill this in...
    //
    // Set up the lighting for your scene using m_phongShader.
    // The lighting information will most likely be stored in
    // CS123SceneLightData structures.
    //
    for (auto light : m_lights) {
        m_phongShader->setLight(light);
    }
}

void SceneviewScene::renderGeometry() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // TODO: [SCENEVIEW] Fill this in...
    // You shouldn't need to write *any* OpenGL in this class!
    //
    //
    // This is where you should render the geometry of the scene. Use what you
    // know about OpenGL and leverage your Shapes classes to get the job done.
    //

    for (CS123TransformPrimitive &transPrim : m_transPrims) {
        glm::mat4x4 transform = transPrim.transform;
        CS123SceneMaterial material = transPrim.primitive.material;

        m_phongShader->setUniform("m", transform);
        m_phongShader->applyMaterial(material);

        // Handle texture
        if (material.textureMap.isUsed && !material.textureMap.filename.empty()) {
            loadMapData(material);
            tryApplyTexture(material.textureMap);
        }

        if (!m_cube || !m_cone || !m_cylinder || !m_sphere || !m_torus)
            resetPrimitivePrototypes();

        PrimitiveType type = transPrim.primitive.type;

        switch (type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_cube->draw();
            break;

        case PrimitiveType::PRIMITIVE_CONE:
            m_cone->draw();
            break;

        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder->draw();
            break;

        case PrimitiveType::PRIMITIVE_SPHERE:
            m_sphere->draw();
            break;

        case PrimitiveType::PRIMITIVE_TORUS:
            m_torus->draw();
            break;

        case PrimitiveType::PRIMITIVE_MESH:
            // Temporarily use cube
            m_cube->draw();
            break;

        default:
            qDebug("Invalid primitive type!");
            exit(-1);
        }
    }
}

void SceneviewScene::loadMapData(CS123SceneMaterial& mat) {
    if (m_textures.find(mat.textureMap.filename) != m_textures.end())
        return;

    QImage img(mat.textureMap.filename.data());
    img = QGLWidget::convertToGLFormat(img);

    if (img.isNull())
        return;

    Texture2D tex(img.bits(), img.width(), img.height());
    buildTexture(tex);
    m_textures.emplace(mat.textureMap.filename, std::move(tex));
}

void SceneviewScene::buildTexture(Texture2D &tex) {
    TextureParametersBuilder builder;
    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
    TextureParameters params = builder.build();
    params.applyTo(tex);
}

void SceneviewScene::tryApplyTexture(CS123SceneFileMap& map) {
    if (!map.isUsed) {
        m_phongShader->setUniform("useTexture", 0);
        return;
    }
    m_phongShader->setUniform("useTexture", 1);
    m_phongShader->setUniform("repeatUV", glm::vec2(map.repeatU, map.repeatV));
    m_phongShader->setTexture("tex", m_textures.at(map.filename));
}

void SceneviewScene::resetPrimitivePrototypes() {
    int p1 = glm::ceil(settings.shapeParameter1 * m_lod_coef), p2 = glm::ceil(settings.shapeParameter2 * m_lod_coef);
    float p3 = settings.shapeParameter3 * m_lod_coef;
    m_cube = std::make_unique<Cube>(p1, p2, p3);
    m_cone = std::make_unique<Cone>(p1, p2, p3);
    m_torus = std::make_unique<Torus>(p1, p2, p3);
    m_sphere = std::make_unique<Sphere>(p1, p2, p3);
    m_cylinder = std::make_unique<Cylinder>(p1, p2, p3);
    m_initialized = true;
}

void SceneviewScene::settingsChanged() {
    // TODO: [SCENEVIEW] Fill this in if applicable.
    resetPrimitivePrototypes();
}
