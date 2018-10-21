#ifndef OPENGLSHAPE_H
#define OPENGLSHAPE_H

#include "GL/glew.h"

#include <memory>
#include <vector>

#include "gl/datatype/VBO.h"
#include "gl/datatype/VBOAttribMarker.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "glm.hpp"

#define PI 3.1415926f

namespace CS123 { namespace GL {
class VAO;
} }

using namespace CS123::GL;

class OpenGLShape
{
public:
    OpenGLShape(int param1, int param2, float param3);
    ~OpenGLShape();

    /* Initialize VBO with given vertex data */
    void setVertexData(GLfloat * data, int size, VBO::GEOMETRY_LAYOUT drawMode, int numVertices);

    /* Enables the specified attribute and calls glVertexAttribPointer with the given arguments. */
    void setAttribute(GLuint index, GLuint numElementsPerVertex, int offset, VBOAttribMarker::DATA_TYPE type,
                      bool normalize);

    /* Build the VAO given the specified vertex data and attributes */
    void buildVAO();

    /* Draw the initialized geometry. */
    void draw();

    /* LOOK HERE: Added member methods. */
    virtual void reCalculateVertices();
    virtual void populateCoordinates(std::vector<glm::vec4>& vertices);
    int getP1();
    int getP2();
    float getP3();
    void setP1(int p1);
    void setP2(int p2);
    void setP3(float p3);

protected:

    GLfloat * m_data;                       /// Vector of floats containing the vertex data.
    GLsizeiptr m_size;                      /// Size of the data array, in floats.
    VBO::GEOMETRY_LAYOUT m_drawMode;        /// Drawing mode
    int m_numVertices;                      /// Number of vertices to be rendered.
    std::vector<VBOAttribMarker> m_markers; /// List of VBOAttribMarkers that describe how data is laid out.
    std::unique_ptr<CS123::GL::VAO> m_VAO;  /// A wrapper for the vertex array object (VAO)

    std::vector<float> m_coords;
    int m_p1, m_p2;
    float m_p3;
    float m_radius;
    bool m_needRecalculate;

};

#endif // OPENGLSHAPE_H
