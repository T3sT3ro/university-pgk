//
// Created by tooster on 18.12.2019.
//

#include "Mesh.hpp"

#include <utility>
#include <algorithm>
#include <fstream>
#include <sstream>

Mesh::Mesh(std::vector<vec3> &points, std::vector<GLushort> &indices, GLenum usage, GLenum mode)
        : vbo(-1), idx(-1), points(points), indices(indices), mode(mode), usage(usage) {

    glGenBuffers(1, &vbo); // vertices buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * points.size(), points.data(), usage);

    glGenBuffers(1, &idx); // indices buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), usage);
    VERBOSE(CDBG << "[Mesh] created: "
                      "[pts:"<<points.size()
                    <<",idx:"<<indices.size()
                    <<",vbo:" << vbo
                    <<",idx:" << idx << "]\n");
}

Mesh *Mesh::create(vector<vec3> &points, vector<GLushort> &indices, GLenum usage, GLenum mode) {
    return new Mesh(points, indices, usage, mode);
}

Mesh *Mesh::create(const char *objPath, GLenum usage, GLenum mode) {
    ifstream     objFile;
    stringstream objStream;

    objFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        objFile.open(objPath);
        string           line;
        vector<vec3>     points;
        vector<GLushort> indices;

        while (std::getline(objFile, line)) {
            if (line.substr(0, 1) == "#") continue;
            istringstream ss(line);
            string        what;
            ss >> what;
            if (what == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                points.emplace_back(x, y, z);
            } else if (what == "f") {
                GLushort u, v, w;
                ss >> u >> v >> w;
                indices.emplace_back(u - 1);
                indices.emplace_back(v - 1);
                indices.emplace_back(w - 1);
            }
        }
        objFile.close();
        return Mesh::create(points, indices, usage, mode);
    } catch (ifstream::failure &e) {
        cerr << "[Mesh] error while loading obj file:\n" << e.what() << endl;
        return nullptr;
    }
}

void Mesh::use() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &idx);
}