//
// Created by tooster on 18.12.2019.
//

#include "Mesh.hpp"

#include <utility>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>

using namespace std;


Mesh::Mesh(GLenum usage, GLenum mode) : mode(mode), usage(usage) {}

static regex faceVertexRegex(R"((\d+)\/?(\d+)?\/?(\d+)?)");

Mesh *Mesh::import(const char *objPath, GLenum usage, GLenum mode) {
    ifstream     objFile;
    stringstream objStream;

    auto mesh = new Mesh(usage, mode);
    objFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        objFile.open(objPath);
        string line;

        std::vector<vec3> rawVertices; // read rawVertices
        std::vector<vec2> rawUVs; // read texcords
        std::vector<vec3> rawNormals; // read normals

        std::vector<Vertex>                      vertices; // unique rawVertices data
        std::vector<GLushort>                    indices; // index into unique rawVertices
        std::map<tuple<int, int, int>, GLushort> faceVertices; // map to generate unique indices

        // init rawVertices, normals, uvs and faces
        int lineNO = 0;
        while (!objFile.eof() && std::getline(objFile, line)) {
            ++lineNO;
            istringstream ss(line);
            string        what;
            ss >> what;
            if (what[0] == '#') continue;
            else if (what == "v") { /// w value is ignored
                float x, y, z;
                ss >> x >> y >> z;
                rawVertices.emplace_back(x, y, z);
            } else if (what == "vt") { // if UVs are not present, they are ignored
                mesh->hasUVs = true;
                GLushort u, v;
                ss >> u >> v;
                rawUVs.emplace_back(u, v);
            } else if (what == "vn") { /// if normals are not present, they are ignored
                mesh->hasNormals = true;
                GLushort x, y, z;
                ss >> x >> y >> z;
                rawNormals.emplace_back(x, y, z);
            } else if (what == "f") {
                mesh->facesCount++;
                while(!ss.eof()){
                    string faceVertex;
                    ss >> faceVertex;
                    smatch match;
                    if(regex_match(faceVertex, match, faceVertexRegex) && match.size() == 4){

                        // translate .obj 1-indexed variables to 0-indexed and negative to indices from the end
                        int v = stoi(match[1]), vt = stoi(match[2]), vn = stoi(match[3]);
                        v  = v < 0 ? rawVertices.size() - v : v-1;
                        vt = vt < 0 ? rawUVs.size() - vt : vt-1;
                        vn = vn < 0 ? rawNormals.size() - vn : vn-1;

                        auto key = make_tuple(v,vt,vn);

                        if(faceVertices.count(key) == 0){ // if this unique vertex didn't exist
                            int newIndex = faceVertices.size(); // assign new unique index to it
                            indices.push_back(newIndex);  // add it's index to element array
                            faceVertices[key] = newIndex; // mark vertex as met
                            vertices.push_back({rawVertices[get<0>(key)],
                                                mesh->hasUVs ? rawUVs[get<1>(key)] : decltype(Vertex::uv)(0),
                                                mesh->hasNormals ? rawNormals[get<2>(key)] : decltype(Vertex::normal)(0)});
                        } else // if exists the same unique vertex
                            indices.push_back(faceVertices[key]); // add index of super vertex
                        mesh->verticesCount++; // FIXME FUCKING FIXME
                    } else {
                        VERBOSE_LOW(cerr << "[Mesh] invalid vertex format encountered: " << faceVertex << endl);
                    }
                }
            } else if(!line.empty()){
                VERBOSE_LOW(cerr << "[Mesh] unrecognized line:" << lineNO << ": '" << line << "' in " << objPath << endl);
            }
        }
        objFile.close();

        // TODO: squash unused vertex parameters
        glGenBuffers(1, &mesh->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), mesh->usage);

        glGenBuffers(1, &mesh->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(decltype(indices)::value_type)*indices.size(), indices.data(), mesh->usage);

        // cleanup so others won't overwrite
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return mesh;

    } catch (ifstream::failure &e) {
        cerr << "[Mesh] error while loading obj file:\n" << e.what() << endl;
        objFile.close();
        delete mesh;
        exit(0);
    }
}

void Mesh::use(Shader *shader) {
    GLint vAttrib = glGetAttribLocation(shader->getProgID(), V_ATTRIB_NAME);
    GLint vtAttrib = glGetAttribLocation(shader->getProgID(), VT_ATTRIB_NAME);
    GLint vnAttrib = glGetAttribLocation(shader->getProgID(), VN_ATTRIB_NAME);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if(vAttrib != -1){
        glEnableVertexAttribArray(vAttrib);
        glVertexAttribPointer(vAttrib, decltype(Vertex::position)::length(), GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
    } else {
        glDisableVertexAttribArray(vAttrib);
        cerr << "[Mesh] missing attribute `" V_ATTRIB_NAME "` in shader " << shader->getName() << "(maybe optimized-out?)\n";
    }

    if(hasUVs && vtAttrib != -1){
        glEnableVertexAttribArray(vtAttrib);
        glVertexAttribPointer(vtAttrib, decltype(Vertex::uv)::length(), GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, uv)));
    } else if(hasUVs) {
        glDisableVertexAttribArray(vtAttrib);
        cerr << "[Mesh] missing attribute `" VT_ATTRIB_NAME "` in shader " << shader->getName() << "(maybe optimized-out?)\n";
    }

    if(hasNormals && vnAttrib != -1){
        glEnableVertexAttribArray(vnAttrib);
        glVertexAttribPointer(vnAttrib, decltype(Vertex::normal)::length(), GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    } else if(hasNormals) {
        glDisableVertexAttribArray(vnAttrib);
        cerr << "[Mesh] missing attribute `" VN_ATTRIB_NAME "` in shader " << shader->getName() << "(maybe optimized-out?)\n";
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Mesh::~Mesh() {
    if(VBO)  glDeleteBuffers(1, &VBO);
    if(EBO)  glDeleteBuffers(1, &EBO);
}
