//
// Created by Luke on 26/08/2025.
//

#include "TextureManager.h"

#include "../utils/Assert.h"

void TextureManager::load_textures() {
    load_texture("blocks/grass.png");
    // load_texture("blocks/stone.png");
}

void TextureManager::load_texture(const std::string &texture_name) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    auto file_name = "assets/" + texture_name;

    unsigned char *data = stbi_load(file_name.c_str(), &width, &height, &nrChannels, 0);
    ASSERT(data, "Failed to load texture " << texture_name << " because " << stbi_failure_reason());
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
}