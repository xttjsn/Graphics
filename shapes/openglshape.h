#ifndef OPENGLSHAPE_H
#define OPENGLSHAPE_H

#include "GL/glew.h"

#include <memory>
#include <vector>

#include "gl/datatype/VBO.h"
#include "gl/datatype/VBOAttribMarker.h"

namespace CS123 { namespace GL {
class VAO;
}}

using namespace CS123::GL;

class OpenGLShape
{
public:
    OpenGLShape(int param1, int param2, int param3);
    ~OpenGLShape();

    /* Initialize VBO with given vertex data */
    void setVertexData(GLfloat *data, int size, VBO::GEOMETRY_LAYOUT drawMode, int numVertices);

    /* Enables the specified attribute and calls glVertexAttribPointer with the given arguments. */
    void setAttribute(GLuint index, GLuint numElementsPerVertex, int offset, VBOAttribMarker::DATA_TYPE type, bool normalize);

    /* Build the VAO given the specified vertex data and attributes */
    void buildVAO();

    /* Draw the initialized geometry. */
    void draw();

    /* LOOK HERE: Added member methods  */
    virtual void reCalculateVertices();

private:

    GLfloat *m_data;                            /// Vector of floats containing the vertex data.
    GLsizeiptr m_size;                          /// Size of the data array, in bytes.
    VBO::GEOMETRY_LAYOUT m_drawMode;            /// Drawing mode
    int m_numVertices;                          /// Number of vertices to be rendered.
    std::vector<VBOAttribMarker> m_markers;     /// List of VBOAttribMarkers that describe how data is laid out.
    std::unique_ptr<CS123::GL::VAO> m_VAO;      /// A wrapper for the vertex array object (VAO)

    bool m_needRecalculate;
    int m_p1, m_p2, m_p3;

    friend class ShapesScene;
};

#endif // OPENGLSHAPE_H
