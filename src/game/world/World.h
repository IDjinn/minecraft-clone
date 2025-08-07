//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
#include <array>

#include "chunks/Chunk.h"
#include "glm/vec3.hpp"

#include "WorldConstants.h"

struct World {
    uint8_t id;
    glm::vec3 spawn_point;

    std::array<Chunk, WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z> chunks {};

    World(uint8_t id, const glm::vec3 &spawn_point) : id(id), spawn_point(spawn_point){
    }

    static constexpr int chunk_index(const int x, const int y, const int z) {
        return x + (WORLD_SIZE_X * y) + (WORLD_SIZE_X * WORLD_SIZE_Y * z);
    }

    [[nodiscard]] Chunk getChunk(int x, int y, int z) const;

  [[nodiscard]]  Chunk getChunk(uint8_t chunk_index) const;

    static constexpr bool isOutOfBounds(const int x, const int y, const int z) {
        return x >= WORLD_SIZE_X || y >= WORLD_SIZE_Y || z >= WORLD_SIZE_Z || x < 0 || y < 0 || z < 0;
    }
};


#endif //MINECRAFT_WORLD_H
