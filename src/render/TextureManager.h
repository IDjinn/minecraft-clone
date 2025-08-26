//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_TEXTUREMANAGER_H
#define MINECRAFT_TEXTUREMANAGER_H
#include <iostream>
#include <string>

#include "glad/glad.h"
#include "stb_image/stb_image.h"

struct TextureManager {
    static void load_texture(const std::string &texture_name);
};


#endif //MINECRAFT_TEXTUREMANAGER_H