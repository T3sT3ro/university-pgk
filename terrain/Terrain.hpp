//
// Created by tooster on 13.02.2020.
//

#ifndef PGK_TERRAIN_HPP
#define PGK_TERRAIN_HPP


#include "common.hpp"
#include "Mesh.hpp"

constexpr const int TERRAIN_SAMPLES         = 1201;
constexpr const int TERRAIN_PARALLELS_COUNT = 180;
constexpr const int TERRAIN_MERIDIANS_COUNT = 360;
constexpr const int TERRAIN_SLICES          = 5;

class Terrain : public Mesh{

    Terrain(const char *path, int x, int y);

    // LODs are a multiple of 2
    constexpr static size_t LOD_LEVELS = 4; // LOD_0 is the most detailed
    constexpr static size_t SLICE_SIZE = 128;

    struct TerrainSlice {
        GLuint dataBuffer = 0;
        TerrainSlice(vector<GLshort> &heights);
    } terrainSlices[TERRAIN_SLICES][TERRAIN_SLICES];

    GLuint lodEBO[LOD_LEVELS];

    int enabledLods = 1 << LOD_LEVELS - 1;
    int mapX =0, mapY =0; // which slice is the center of map
    int sliceXOffset =TERRAIN_SAMPLES/2, sliceYOffset=TERRAIN_SAMPLES/2; // how to offset mesh along
    int centerSliceX = TERRAIN_SLICES/2, centerSliceY = TERRAIN_SLICES/2;

    static char* mapsDirectory;

public:
    struct TerrainCoord {
        GLshort x;
        GLshort y;
    };

    static Terrain *import(const char *hgtDir, int longitude, int latitude);

    /**
     * Rebuild the terrain mesh based on LOD settings
     */
    void recreateMesh();

    void use(Shader *shader) override;

    void fetchSlice(int x, int y, TerrainSlice &slice);

    TerrainSlice& getRelativeSlice(int x, int y);

    ~Terrain() override;
};


#endif //PGK_TERRAIN_HPP
