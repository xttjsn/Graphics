#include "openglshape.h"

#include "gl/datatype/VAO.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "shapes/shapeutil.h"
#include <QtGlobal>

using namespace CS123::GL;

OpenGLVertex::OpenGLVertex(glm::vec4 pos, glm::vec4 norm, glm::vec2 texc) :
    position(pos), normal(norm), texcoord(texc) {
}

OpenGLVertex OpenGLVertex::rotate(glm::mat4x4 rot) {
    // Only change pos and norm, keep texcoord
    glm::vec4 new_pos  = rot * position;
    glm::vec4 new_norm = rot * normal;
    return OpenGLVertex(new_pos, new_norm, texcoord);
}

OpenGLVertex OpenGLVertex::sphericalRotate(glm::mat4x4 rot) {
    // Change all
    ShapeUtil shapeutil;
    glm::vec4 new_pos = rot * position;
    glm::vec4 new_norm = rot * normal;
    glm::vec2 new_uv = shapeutil.sphericalUV(new_pos);
    return OpenGLVertex(new_pos, new_norm, new_uv);
}

OpenGLVertex OpenGLVertex::normAvgWithRotation(glm::mat4x4 rot) {
    // Average normal with the one after roration
    ShapeUtil shapeutil;
    glm::vec4 new_norm = rot * normal;
    return OpenGLVertex(position, glm::normalize(shapeutil.interpolate(normal, new_norm, 0.5f)), texcoord);
}

OpenGLShape::OpenGLShape(int p1, int p2, float p3) :
    m_size(0),
    m_drawMode(VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLES),
    m_numVertices(0),
    m_VAO(nullptr),
    m_p1(p1),
    m_p2(p2),
    m_p3(p3),
    m_radius(0.5),
    m_needRecalculate(true){
    // You shouldn't need to do anything here
    // Aren't initializer lists amazing?
}

OpenGLShape::~OpenGLShape(){
    // You shouldn't need to do anything here
    // m_VAO's destructor will automatically be called
    // Aren't smart pointers amazing?
}

/**
 * @param data - Vector of floats containing the vertex data.
 * @param size - number of elements in the array.
 * @param drawMode - Drawing mode
 * @param numVertices - Number of vertices to be rendered.
 */
void OpenGLShape::setVertexData(GLfloat * data, int size, VBO::GEOMETRY_LAYOUT drawMode, int numVertices){
    // Store the vertex data and other values to be used later when constructing the VAO
    m_data        = data;
    m_size        = size;
    m_drawMode    = drawMode;
    m_numVertices = numVertices;
}

/**
 * @param index OpenGL handle to the attribute location. These are specified in ShaderAttribLocations.h
 * @param numElementsPerVertex Number of elements per vertex. Must be 1, 2, 3 or 4 (e.g. position = 3 for x,y,z)
 * @param offset Offset in BYTES from the start of the array to the beginning of the first element
 * @param type Primitive type (FLOAT, INT, UNSIGNED_BYTE)
 * @param normalize
 */
void OpenGLShape::setAttribute(
    GLuint index, GLuint numElementsPerVertex, int offset, VBOAttribMarker::DATA_TYPE type, bool normalize){
    m_markers.push_back(VBOAttribMarker(index, numElementsPerVertex, offset, type, normalize));
}

void OpenGLShape::buildVAO(){
    CS123::GL::VBO vbo = VBO(m_data, m_size, m_markers, m_drawMode);
    m_VAO = std::make_unique<VAO>(vbo, m_numVertices);
}

void OpenGLShape::draw(){
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}

/* LOOK HERE: methods implementation */

void OpenGLShape::reCalculateVertices(){
}

void OpenGLShape::populateCoordinates(std::vector<glm::vec4>& vertices){
    for (glm::vec4& v : vertices) {
        m_coords.push_back(v.x);
        m_coords.push_back(v.y);
        m_coords.push_back(v.z);
    }

    static constexpr int kFloatsPerVertex = 6;
    m_numVertices = m_coords.size() / kFloatsPerVertex;
    setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
    setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    buildVAO();

    m_needRecalculate = false;
}

void OpenGLShape::populateCoordinatesUV(std::vector<OpenGLVertex>& vertices) {
    for (OpenGLVertex& v : vertices) {
        m_coords.push_back(v.position.x);
        m_coords.push_back(v.position.y);
        m_coords.push_back(v.position.z);
        m_coords.push_back(v.normal.x);
        m_coords.push_back(v.normal.y);
        m_coords.push_back(v.normal.z);
        m_coords.push_back(v.texcoord.x);
        m_coords.push_back(v.texcoord.y);
    }

    static constexpr int kFloatsPerVertex = 8;
    m_numVertices = m_coords.size() / kFloatsPerVertex;
    setVertexData(m_coords.data(), m_coords.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, m_numVertices);
    setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    setAttribute(ShaderAttrib::TEXCOORD0, 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    buildVAO();

    m_needRecalculate = false;
}

int OpenGLShape::getP1(){
    return m_p1;
}

int OpenGLShape::getP2(){
    return m_p2;
}

float OpenGLShape::getP3(){
    return m_p3;
}

void OpenGLShape::setP1(int p1){
    if (m_p1 == p1) return;

    m_p1              = p1;
    m_needRecalculate = true;
}

void OpenGLShape::setP2(int p2){
    if (m_p2 == p2) return;

    m_p2              = p2;
    m_needRecalculate = true;
}

void OpenGLShape::setP3(float p3){
    if (m_p3 == p3) return;

    m_p3              = p3;
    m_needRecalculate = true;
}
