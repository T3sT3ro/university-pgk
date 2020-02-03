//
// Created by tooster on 29.01.2020.
//

#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture *Texture::currentTexture = nullptr;

Texture::Texture(const char *path) : path(path) {}

void Texture::use() {
    if(currentTexture == nullptr || currentTexture->TEX != 0){
        glBindTexture(GL_TEXTURE_2D, this->TEX);
        currentTexture = this;
    }
}

Texture *Texture::import(const char *texturePath) {
    auto texture = new Texture(texturePath);

    glGenTextures(1, &texture->TEX);
    glBindTexture(GL_TEXTURE_2D, texture->TEX);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    unsigned char *data = stbi_load(texturePath, &texture->width, &texture->height, &texture->nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else{
        std::cout << "[Texture] Failed to load texture " << texturePath << std::endl;
        glDeleteTextures(1, &texture->TEX);
        exit(0); // fixme: change to failsafe method for example returning null
    }
    stbi_image_free(data);
    return nullptr;
}

