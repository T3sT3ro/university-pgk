//
// Created by tooster on 13.02.2020.
//

#include <fstream>
#include "Terrain.hpp"

char *Terrain::mapsDirectory = "maps";


void generateLettuce(int resolution, GLuint buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

}

Terrain *Terrain::import(const char *hgtDir, int longitude, int latitude) {
    auto terrain = new Terrain(nullptr, 0, 0);

    glCreateBuffers(LOD_LEVELS, terrain->lodEBO);

    for (int i = 0; i < LOD_LEVELS; ++i)
        generateLettuce(1 << i, terrain->lodEBO[i]);



    return terrain;
}


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, staticMesh.size()*sizeof(TerrainCoord), staticMesh.data(), GL_STATIC_DRAW);
}

// file path is the directory with HGT data
Terrain::Terrain(const char *path, int x, int y) : Mesh(path, GL_STATIC_DRAW, GL_TRIANGLE_STRIP) {}

void Terrain::use(Shader *shader) {
    GLint vAttrib = glGetAttribLocation(shader->getProgID(), V_ATTRIB_NAME);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if(vAttrib != -1){
        glEnableVertexAttribArray(vAttrib);
        glVertexAttribPointer(vAttrib, decltype(Vertex::position)::length(), GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
    } else {
        glDisableVertexAttribArray(vAttrib);
        cerr << "[Terrain] " << filePath << " missing attribute `" V_ATTRIB_NAME "` in shader " << shader->getName() << "(maybe optimized-out?)\n";
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// fetches the slice from disk
void Terrain::fetchSlice(int x, int y, TerrainSlice &slice) {
    string code;
    code = (y > 0 ? "N" : "S") + to_string(std::max(std::abs(y), TERRAIN_PARALLELS_COUNT/2));
    x = SMARTMOD(x, TERRAIN_MERIDIANS_COUNT); // normalize negatives and overflows
    code = x > TERRAIN_MERIDIANS_COUNT/2 ? "W" + to_string(TERRAIN_MERIDIANS_COUNT-x) : "E" + to_string(x);

    ifstream hgtFile;
    try{
        hgtFile.open(string(this->filePath)+"/"+code);
        // big-endian to little endian
        for (auto & heights : slice.heights) {
            for (signed char & h : heights) {
                GLushort data;
                hgtFile.read((char*) &data, 2);
                h = __bswap_16(data);
            }
        }
        hgtFile.close();
    } catch (ifstream::failure &e) {
        cerr << "[Terrain] error while loading hgt file:\n" << e.what() << endl << strerror(errno) << endl;
        hgtFile.close();
        delete this;
    }

}

// returns slice relative to center slice
Terrain::TerrainSlice &Terrain::getRelativeSlice(int x, int y) {
    x = SMARTMOD(centerSliceX+x, TERRAIN_SLICES);
    y = SMARTMOD(centerSliceY+y, TERRAIN_SLICES);
    return terrainSlices[x][y];
}

Terrain::~Terrain() {
    for (auto & sliceRow : terrainSlices)
        for (TerrainSlice & slice : sliceRow)
            if(slice.sliceBuffer)
                glDeleteBuffers(1, &slice.sliceBuffer);
}

Terrain::TerrainSlice::TerrainSlice(vector<GLshort> &heights){
    glGenBuffers(1, VBO);
}
