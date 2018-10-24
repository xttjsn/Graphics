#include "CS123Shader.h"

#include "CS123SceneData.h"
#include "Settings.h"


#include "gl/GLDebug.h"
#include <sstream>

namespace CS123 { namespace GL {


CS123Shader::CS123Shader(const std::string &vertexSource, const std::string &fragmentSource) :
    Shader(vertexSource, fragmentSource)
{
}

CS123Shader::CS123Shader(const std::string &vertexSource, const std::string &geometrySource, const std::string &fragmentSource) :
    Shader(vertexSource, geometrySource, fragmentSource)
{
}

glm::vec3 toGLMVec3(const CS123SceneColor &c) {
    return glm::vec3(c.r, c.g, c.b);
}

void CS123Shader::applyMaterial(CS123SceneMaterial &material) {
    setUniform("ambient_color", toGLMVec3(material.cAmbient));
    setUniform("diffuse_color", toGLMVec3(material.cDiffuse));
    setUniform("specular_color", toGLMVec3(material.cSpecular));
    setUniform("shininess", material.shininess);

    // Handle texture
    if (material.textureMap.isUsed && !material.textureMap.filename.empty()) {

        if (!material.textureMap.texture.image) {
            material.textureMap.texture.image = std::make_shared<QImage>(QString(material.textureMap.filename.c_str()));
            if (material.textureMap.texture.image->isNull()) {
                fprintf(stderr, "Cannot find texture image");
                exit(-1);
            }
        }
        if (!material.textureMap.texture.textureID) {
            glGenTextures(1, &material.textureMap.texture.textureID);
        }
        glBindTexture(GL_TEXTURE_2D, material.textureMap.texture.textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.textureMap.texture.image->width(),
                     material.textureMap.texture.image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     material.textureMap.texture.image->bits());
        setUniform("repeateUV", glm::vec2(material.textureMap.repeatU, material.textureMap.repeatV));
        glBindTexture(GL_TEXTURE_2D, material.textureMap.texture.textureID);

        setUniform("useTexture", 1);
    } else {
        setUniform("useTexture", 0);
    }
}

void CS123Shader::setLight(const CS123SceneLightData &light) {
    bool ignoreLight = false;

    GLint lightType;
    std::string name;
    glm::vec3 ndir;
    switch(light.type) {
    case LightType::LIGHT_POINT:
        lightType = 0;
        name      = "lightPositions";
        setUniformArrayByIndex(name, light.pos.xyz(), light.id);
        if (!settings.usePointLights) ignoreLight = true;
        break;
    case LightType::LIGHT_DIRECTIONAL:
        lightType = 1;
        ndir      = glm::normalize(light.dir.xyz());
        name      = "lightDirections";
        setUniformArrayByIndex(name, ndir, light.id);
        if (!settings.useDirectionalLights) ignoreLight = true;
        break;
    default:
        lightType   = 0;
        ignoreLight = true;     // Light type not supported
        break;
    }

    CS123SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    setUniformArrayByIndex("lightTypes", lightType, light.id);
    setUniformArrayByIndex("lightColors", glm::vec3(color.r, color.g, color.b), light.id);
//    setUniformArrayByIndex("lightAttenuations", light.function, light.id);
}

}}
