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
#include "../players/Player.h"


struct WorldCoord {
    int32_t x, y, z;
};

struct World {
    uint8_t id;
    glm::vec3 spawn_point;
    std::vector<Player> players;

    std::unordered_map<ChunkId, std::unique_ptr<Chunk> > chunks{};

    World(const uint8_t id, const glm::vec3 &spawn_point) : id(id), spawn_point(spawn_point) {
    }

    void add_player(const Player &player) {
        PRINT_DEBUG(
            "player " << player.name << "(" << player.id << ") added in (" << player.position.x << ", " << player.
            position.y << ", " << player.position.z << ")");
        WHEN_DEBUG(std::cout << std::flush);
        players.push_back(player);
    }

    static uint32_t generate_entity_id() {
        static uint32_t id = 0;
        return ++id;
    }

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

    [[nodiscard]] Chunk &getChunk(WorldCoord coords);

    [[nodiscard]] Chunk &getChunk(ChunkId chunk_id);

    [[nodiscard]] bool isChunkLoaded(ChunkId chunk_id);

    bool loadChunk(ChunkId chunk_id);

    static constexpr bool isOutOfBounds(const int x, const int y, const int z) {
        return x >= WORLD_SIZE_X || y >= WORLD_SIZE_Y || z >= WORLD_SIZE_Z || x < 0 || y < 0 || z < 0;
    }
};


#endif //MINECRAFT_WORLD_H