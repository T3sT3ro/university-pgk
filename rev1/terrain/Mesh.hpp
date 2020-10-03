//
// Created by tooster on 17.11.2019.
//

#ifndef ARKANOID_MESH_HPP
#define ARKANOID_MESH_HPP

#include "common.hpp"
#include "Shader.hpp"
#include <vector>


using namespace glm;
using namespace std;

class Mesh{

    bool hasUVs     = false;
    bool hasNormals = false;


protected:
    /** buffer holding interleaved vertices data*/
    GLuint VBO  = 0;
    /** index buffer for vertices */
    GLuint EBO  = 0;

    const char* filePath;

    Mesh(const char* path, GLenum usage, GLenum mode);

    struct Vertex{
        vec3 position;
        vec2 uv;
        vec3 normal;
    };


public:
    const GLenum mode, usage;
    GLuint       facesCount = 0, verticesCount = 0;

    /**
     * Returns mesh object that has with buffers filled with vertices data
     * @param objPath path to the .obj file.
     * @param usage usage tip for rendering
     * @param mode draw mode for faces !!! if faces have something else than 3 vertices, then change to GL_
     * @return mesh object that will bind and set vertex attribs
     */
    static Mesh *import(const char * objPath, GLenum usage = GL_STATIC_DRAW, GLenum mode = GL_TRIANGLES);

    /***
     * Locates attributes in shader, enables them if they exist and were read and binds the index and vertex buffers.
     * @param shader
     */
    virtual void use(Shader *shader);

    virtual ~Mesh();

};


#endif //ARKANOID_MESH_HPP
