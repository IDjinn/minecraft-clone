//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_SHADERMANAGER_H
#define MINECRAFT_SHADERMANAGER_H

#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <unordered_map>

struct ShaderManager {
    std::unordered_map<std::string, std::string> shaders{};

    const char *load_shader(const std::string &filepath) {
        if (shaders.find(filepath) != shaders.end()) {
            return shaders[filepath].c_str();
        }

        auto shaders_path = "assets/shaders/" + filepath;
        std::ifstream file(shaders_path);
        if (!file.is_open())
            throw std::runtime_error(std::string("Failed to open shader file: ") + shaders_path);

        std::stringstream buffer;
        buffer << file.rdbuf();
        shaders[filepath] = buffer.str();
        return shaders[filepath].c_str();
    }
};


#endif //MINECRAFT_SHADERMANAGER_H
