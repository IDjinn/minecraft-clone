//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include <unordered_set>

#include "../../utils/Assert.h"


World::World(const uint8_t id, const glm::vec3 &spawn_point) : id(id), spawn_point(spawn_point) {
}

void World::add_player(const std::shared_ptr<Player> &player) {
    PRINT_DEBUG(
        "player " << player->name << "(" << player->id << ") added in (" << player->position.x << ", " << player->
        position.y << ", " << player->position.z << ")");
    WHEN_DEBUG(std::cout << std::flush);
    players.push_back(player);
}

void World::check_chunk_lifetimes(const glm::vec3 center_position) {
    const auto world_min_boundary = center_position - WORLD_RENDER_DISTANCE_BLOCKS;
    const auto world_max_boundary = center_position + WORLD_RENDER_DISTANCE_BLOCKS;

    const auto minX = std::max(static_cast<int>(world_min_boundary.x), 0);
    const auto minY = std::max(static_cast<int>(world_min_boundary.y), 0);
    const auto minZ = std::max(static_cast<int>(world_min_boundary.z), 0);

    const auto maxX = static_cast<int>(world_max_boundary.x);
    const auto maxY = static_cast<int>(world_max_boundary.y);
    const auto maxZ = static_cast<int>(world_max_boundary.z);

    std::unordered_set<int32_t> visible_chunks;
    for (auto y = minY; y < maxY; y += CHUNK_SIZE_Y) {
        for (auto x = minX; x < maxX; x += CHUNK_SIZE_X) {
            for (auto z = minZ; z < maxZ; z += CHUNK_SIZE_Z) {
                const auto chunk_id = chunk_id_from_world_coords({x, y, z});
                load_chunk(chunk_id);

                auto &chunk = get_chunk(chunk_id);
                chunk.set_index(chunk_id);

                visible_chunks.insert(chunk_id);
                auto [world_x, world_y, world_z] = chunk_id_to_world_coordinates(chunk_id);
                PRINT_DEBUG(
                    "loaded chunk=" << chunk_id <<" at x=" << x << " y=" << y << " z=" << z << " (" << world_x <<
                    ", "
                    << world_y << ", " << world_z<<")");
                WHEN_DEBUG(std::cout << std::flush);
            }
        }
    }

    std::unordered_set<int32_t> chunks_to_unload;
    for (auto &[chunk_id, chunk]: this->chunks) {
        if (visible_chunks.find(chunk_id) == visible_chunks.end()) {
            chunks_to_unload.insert(chunk_id);
        }
    }

    for (auto chunk_id_to_unload: chunks_to_unload) {
        unload_chunk(chunk_id_to_unload);
    }
}

constexpr int32_t World::chunk_id_from_world_coords(const WorldCoord coord) {
    const auto chunkX = coord.x / CHUNK_SIZE_X;
    const auto chunkY = coord.y / CHUNK_SIZE_Y;
    const auto chunkZ = coord.z / CHUNK_SIZE_Z;

    return chunkX + (WORLD_SIZE_X * chunkY) + (WORLD_SIZE_X * WORLD_SIZE_Y * chunkZ);
}

constexpr WorldCoord World::chunk_id_to_world_coordinates(const int32_t id) {
    const auto chunkZ = id / (WORLD_SIZE_X * WORLD_SIZE_Y);
    const auto remaining = id % (WORLD_SIZE_X * WORLD_SIZE_Y);
    const auto chunkY = remaining / WORLD_SIZE_X;
    const auto chunkX = remaining % WORLD_SIZE_X;

    return {chunkX * CHUNK_SIZE_X, chunkY * CHUNK_SIZE_Y, chunkZ * CHUNK_SIZE_Z};
}

constexpr bool World::isOutOfBounds(const int x, const int y, const int z) {
    return x >= WORLD_SIZE_X || y >= WORLD_SIZE_Y || z >= WORLD_SIZE_Z || x < 0 || y < 0 || z < 0;
}

uint32_t World::generate_entity_id() {
    static uint32_t id = 0;
    return ++id;
}

std::unique_ptr<std::vector<float> > World::generate_visible_vertices() {
    auto vertices = std::make_unique<std::vector<float> >();
    for (auto &[chunk_id, chunk_vertices]: this->chunk_visible_vertices) {
        vertices->insert(vertices->end(), chunk_vertices->begin(), chunk_vertices->end());
    }
    return vertices;
}

Chunk &World::get_chunk(const WorldCoord coords) {
    return get_chunk(chunk_id_from_world_coords(coords));
}

Chunk &World::get_chunk(int32_t chunk_id) {
    auto it = chunks.find(chunk_id);
    ASSERT(it != chunks.end(), "Chunk was not found/loaded");
    return *it->second;
}

bool World::isChunkLoaded(int32_t chunk_id) {
    return chunks.find(chunk_id) != chunks.end();
}

void World::load_chunk(int32_t chunk_id) {
    if (isChunkLoaded(chunk_id)) return;

    chunks[chunk_id] = std::make_unique<Chunk>(chunk_id, shared_from_this());
    chunks[chunk_id]->initialize_blocks();
    this->chunk_visible_vertices[chunk_id] = chunks[chunk_id]->generate_visible_vertices();
}

void World::unload_chunk(const int32_t id) {
    this->chunks.erase(id);
    this->chunk_visible_vertices.erase(id);
    PRINT_DEBUG("unloaded chunk=" << id);
}
