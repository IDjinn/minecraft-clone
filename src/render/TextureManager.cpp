//
// Created by Luke on 26/08/2025.
//

#include "TextureManager.h"

#include "../utils/Assert.h"

void TextureManager::load_texture(const std::string &texture_name) {
    stbi_set_flip_vertically_on_load(1);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(("assets/" + texture_name).c_str(), &width, &height, &nrChannels, 0);
    ASSERT(data, "Failed to load texture " << texture_name << " because " << stbi_failure_reason());

    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
}