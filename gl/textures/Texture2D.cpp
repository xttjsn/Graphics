#include "Texture2D.h"

#include <utility>

namespace CS123 { namespace GL {

Texture2D::Texture2D(unsigned char *data, int width, int height, GLenum type)
{
    GLenum internalFormat = type == GL_FLOAT ? GL_RGBA32F : GL_RGBA;

    // TODO [Task 2]
    // Bind the texture by calling bind() and filling it in
    // Generate the texture with glTexImage2D
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, internalFormat, GL_UNSIGNED_BYTE, data);
    // TODO Don't forget to unbind!
    unbind();
}

void Texture2D::bind() const {
    // TODO [Task 2]
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture2D::unbind() const {
    // TODO Don't forget to unbind!
    glBindTexture(GL_TEXTURE_2D, 0);
}

}}
