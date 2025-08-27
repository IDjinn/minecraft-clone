//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include <unordered_set>

#include "../../utils/Assert.h"
#include "generation/WorldGeneration.h"


World::World(const uint8_t id, const glm::vec3 &spawn_point) : id(id),
                                                               spawn_point(spawn_point),
                                                               world_generation(
                                                                   std::make_unique<WorldGeneration>(1234l)) {
}

void World::add_player(const std::shared_ptr<Player> &player) {
    PRINT_DEBUG(
        "player " << player->name << "(" << player->id << ") added in (" << player->position.x << ", " << player->
        position.y << ", " << player->position.z << ")");
    WHEN_DEBUG(std::cout << std::flush);
    players.push_back(player);
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

void World::check_chunk_lifetimes(const glm::vec3 center_position) {
    std::unordered_set<int32_t> chunks_to_unload{};
    auto visible_chunks = world_generation->generate_chunks_around(
        this->shared_from_this(), center_position);

    for (auto &[chunk_id, chunk]: *visible_chunks) {
        this->chunks[chunk_id] = std::move(chunk);
    }

    for (auto &[chunk_id, chunk]: this->chunks) {
        if (visible_chunks->find(chunk_id) == visible_chunks->end()) {
            chunks_to_unload.insert(chunk_id);
            continue;
        }

        this->chunk_visible_vertices[chunk_id] = chunk->generate_visible_vertices();
    }

    for (const auto chunk_id_to_unload: chunks_to_unload) {
        unload_chunk(chunk_id_to_unload);
    }
}

void World::unload_chunk(const int32_t id) {
    this->chunks.erase(id);
    this->chunk_visible_vertices.erase(id);
    PRINT_DEBUG("unloaded chunk=" << id);
    WHEN_DEBUG(std::cout << std::flush);
}

bool World::is_chunk_loaded(int32_t chunk_id) {
    return chunks.find(chunk_id) != chunks.end();
}

Chunk &World::get_chunk(int32_t chunk_id) {
    auto it = chunks.find(chunk_id);
    ASSERT(it != chunks.end(), "Chunk was not found/loaded");
    return *it->second;
}

Chunk &World::get_chunk(const WorldCoord coords) {
    return get_chunk(world_coords_to_chunk_id(coords));
}
