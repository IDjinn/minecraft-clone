//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
#include <array>
#include <map>
#include <unordered_map>
#include <vector>

#include "chunks/Chunk.h"
#include "glm/vec3.hpp"

#include "WorldConstants.h"

struct WorldCoord {
    int32_t x, y, z;
};

struct World {
    uint8_t id;
    glm::vec3 spawn_point;

    std::unordered_map<ChunkId, std::unique_ptr<Chunk> > chunks{};

    World(uint8_t id, const glm::vec3 &spawn_point) : id(id), spawn_point(spawn_point) {
    }

    // static constexpr ChunkId chunk_id(const int32_t x, const int32_t y, const int32_t z) {
    //     return x + (WORLD_SIZE_X * y) + (WORLD_SIZE_X * WORLD_SIZE_Y * z);
    // }
    //
    // static constexpr std::tuple<int32_t, int32_t, int32_t> chunk_id_to_coordinates(const ChunkId chunk_id) {
    //     const auto z = chunk_id / (WORLD_SIZE_X * WORLD_SIZE_Y);
    //     const auto remaining = chunk_id % (WORLD_SIZE_X * WORLD_SIZE_Y);
    //     const auto y = remaining / WORLD_SIZE_X;
    //     const auto x = remaining % WORLD_SIZE_X;
    //
    //     return {x, y, z};
    // }

    static constexpr ChunkId chunk_id_from_world_coords(const WorldCoord coord) {
        const auto chunkX = coord.x / CHUNK_SIZE_X;
        const auto chunkY = coord.y / CHUNK_SIZE_Y;
        const auto chunkZ = coord.z / CHUNK_SIZE_Z;

        return chunkX + (WORLD_SIZE_X * chunkY) + (WORLD_SIZE_X * WORLD_SIZE_Y * chunkZ);
    }

    static constexpr WorldCoord chunk_id_to_world_coordinates(const ChunkId id) {
        const auto chunkZ = id / (WORLD_SIZE_X * WORLD_SIZE_Y);
        const auto remaining = id % (WORLD_SIZE_X * WORLD_SIZE_Y);
        const auto chunkY = remaining / WORLD_SIZE_X;
        const auto chunkX = remaining % WORLD_SIZE_X;

        return {chunkX * CHUNK_SIZE_X, chunkY * CHUNK_SIZE_Y, chunkZ * CHUNK_SIZE_Z};
    }


    // static constexpr std::tuple<int32_t, int32_t, int32_t> chunk_id_to_world_coords(const ChunkId id) {
    //     auto [chunkX, chunkY, chunkZ] = chunk_id_to_coordinates(id);
    //     return {chunkX * CHUNK_SIZE_X, chunkY * CHUNK_SIZE_Y, chunkZ * CHUNK_SIZE_Z};
    // }

    [[nodiscard]] Chunk &getChunk(WorldCoord coords);

    [[nodiscard]] Chunk &getChunk(ChunkId chunk_id);

    [[nodiscard]] bool isChunkLoaded(ChunkId chunk_id);

    bool loadChunk(ChunkId chunk_id);

    static constexpr bool isOutOfBounds(const int x, const int y, const int z) {
        return x >= WORLD_SIZE_X || y >= WORLD_SIZE_Y || z >= WORLD_SIZE_Z || x < 0 || y < 0 || z < 0;
    }
};


#endif //MINECRAFT_WORLD_H
