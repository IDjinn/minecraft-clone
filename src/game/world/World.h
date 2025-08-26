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
    std::vector<std::shared_ptr<Player> > players;

    std::unordered_map<ChunkId, std::unique_ptr<Chunk> > chunks{};

    World(const uint8_t id, const glm::vec3 &spawn_point) : id(id), spawn_point(spawn_point) {
        generate_chunks();
    }

    void add_player(const std::shared_ptr<Player> &player) {
        PRINT_DEBUG(
            "player " << player->name << "(" << player->id << ") added in (" << player->position.x << ", " << player->
            position.y << ", " << player->position.z << ")");
        WHEN_DEBUG(std::cout << std::flush);
        players.push_back(player);
    }

    void generate_chunks() {
        const glm::vec3 spawn_coords = WORLD_SPAWN_COORDS;
        const auto world_min_boundary = spawn_coords - static_cast<float>(WORLD_SPAWN_RENDER_CHUNKS);
        const auto world_max_boundary = spawn_coords + static_cast<float>(WORLD_SPAWN_RENDER_CHUNKS);

        const auto minX = std::max(static_cast<int>(world_min_boundary.x), 0);
        const auto minY = std::max(static_cast<int>(world_min_boundary.y), 0);
        const auto minZ = std::max(static_cast<int>(world_min_boundary.z), 0);

        const auto maxX = static_cast<int>(world_max_boundary.x);
        const auto maxY = static_cast<int>(world_max_boundary.y);
        const auto maxZ = static_cast<int>(world_max_boundary.z);

        for (auto y = minY; y < maxY; y += CHUNK_SIZE_Y) {
            for (auto x = minX; x < maxX; x += CHUNK_SIZE_X) {
                for (auto z = minZ; z < maxZ; z += CHUNK_SIZE_Z) {
                    const auto chunk_id = World::chunk_id_from_world_coords({x, y, z});
                    auto chunkLoaded = loadChunk(chunk_id);
                    ASSERT_DEBUG(chunkLoaded, "failed loading chunk");
                    if (!chunkLoaded) continue;

                    auto [world_x, world_y, world_z] = World::chunk_id_to_world_coordinates(chunk_id);
                    PRINT_DEBUG(
                        "loaded chunk=" << chunk_id <<" at x=" << x << " y=" << y << " z=" << z << " (" << world_x << ", "
                        << world_y << ", " << world_z<<")");
                    WHEN_DEBUG(std::cout << std::flush);

                    auto &chunk = getChunk(chunk_id);
                    chunk.setIndex(chunk_id);
                    chunk.initializeBlocks();
                }
            }
        }
    }

    std::vector<float> generate_visible_vertices() {
        std::vector<float> visibleVertices;
        visibleVertices.reserve(WORLD_RENDER_VERTICES_RESERVE);

        ASSERT_DEBUG(chunks.size()>0, "invalid world chunks");
        for (auto &[chunk_index, chunk]: chunks) {
            const auto [chunk_x, chunk_y, chunk_z] = chunk_id_to_world_coordinates(chunk_index);

            // chunk coordinate system tests
            {
                WHEN_DEBUG(const auto chunk_id = chunk_id_from_world_coords({chunk_x, chunk_y, chunk_z}));
                ASSERT_DEBUG(chunk_index == chunk_id, "Mismatch chunking id => coordinates");
            }
            //

            for (auto y = 0; y < CHUNK_SIZE_Y; y++) {
                for (auto x = 0; x < CHUNK_SIZE_X; x++) {
                    for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
                        auto index = Chunk::block_index(x, y, z);
                        if (auto &block = chunk->blocks[index]; block.block_type() == BlockType::AIR) continue;

                        for (auto face = 0; face < 6; ++face) {
                            auto nx = x + directions[face][0];
                            auto ny = y + directions[face][1];
                            auto nz = z + directions[face][2];

                            auto neighborIsSolid = false;
                            const auto world_position_current_block = glm::vec3(chunk_x + x, chunk_y + y, chunk_z + z);
                            if (nx >= 0 && nx < CHUNK_SIZE_X &&
                                ny >= 0 && ny < CHUNK_SIZE_Y &&
                                nz >= 0 && nz < CHUNK_SIZE_Z) {
                                auto neighbor_block_index = Chunk::block_index(nx, ny, nz);
                                neighborIsSolid = chunk->blocks[neighbor_block_index].block_type() != BlockType::AIR;
                            } else {
                                auto neighbor_x = x + directions[face][0];
                                auto neighbor_y = y + directions[face][1];
                                auto neighbor_z = z + directions[face][2];
                                auto neighbor_chunk_id = chunk_id_from_world_coords(
                                    {chunk_x + neighbor_x, chunk_y + neighbor_y, chunk_z + neighbor_z}
                                );

                                if (auto it = chunks.find(neighbor_chunk_id); it != chunks.end()) {
                                    auto &neighbor_chunk = it->second;
                                    auto neighbor_block_x = (neighbor_x + CHUNK_SIZE_X) % CHUNK_SIZE_X;
                                    auto neighbor_block_y = (neighbor_y + CHUNK_SIZE_Y) % CHUNK_SIZE_Y;
                                    auto neighbor_block_z = (neighbor_z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
                                    auto neighbor_index = Chunk::block_index(
                                        neighbor_block_x, neighbor_block_y, neighbor_block_z);
                                    neighborIsSolid =
                                            neighbor_chunk->blocks[neighbor_index].block_type() != BlockType::AIR;
                                }
                            }

                            if (neighborIsSolid) continue;
                            for (int i = 0; i < 30; i += 5) {
                                auto vx = faceVertices[face][i] + world_position_current_block.x;
                                auto vy = faceVertices[face][i + 1] + world_position_current_block.y;
                                auto vz = faceVertices[face][i + 2] + world_position_current_block.z;
                                auto u = faceVertices[face][i + 3];
                                auto v = faceVertices[face][i + 4];
                                visibleVertices.insert(visibleVertices.end(), {vx, vy, vz, u, v});
                            }
                        }
                    }
                }
            }
        }
        return visibleVertices;
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
