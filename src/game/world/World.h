//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
#include <array>
#include <cstdint>
#include <optional>

#include "chunks/Chunk.h"
#include "glm/vec3.hpp"

#define WORLD_SIZE_X 32
#define WORLD_SIZE_Y 4

class World {
    uint8_t id;
    glm::vec3 spawn_point;

    std::array<std::array<Chunk, WORLD_SIZE_Y>, WORLD_SIZE_X> chunks = {};

    const std::optional<Chunk&> getChunk(uint64_t id) {
        for (auto& arr : chunks)
            for (auto& chunk : arr)
                if (chunk.id == id)
                    return std::optional<Chunk&>(chunk);

        return {};
    }

};


#endif //MINECRAFT_WORLD_H