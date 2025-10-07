//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include <unordered_set>

#include "../../utils/Assert.h"
#include "chunks/Chunk.h"
#include "chunks/Chunk.h"
#include "generation/WorldGeneration.h"


World::World(const uint8_t id, const glm::vec3 &spawn_point) : id(id),
                                                               spawn_point(spawn_point),
                                                               world_generation(
                                                                   std::make_unique<WorldGeneration>(1234l)) {
}

World::~World() {
    DEBUG_BREAK();
}

void World::add_player(const std::shared_ptr<Player> &player) {
    PRINT_DEBUG(
        "player " << player->name << "(" << player->id << ") added in (" << player->position.x << ", " << player->
        position.y << ", " << player->position.z << ")" << std::flush);
    players.push_back(player);
}

uint32_t World::generate_entity_id() {
    static uint32_t id = 0;
    return ++id;
}

std::unique_ptr<std::vector<float> > World::generate_visible_vertices() {
    auto vertices = std::make_unique<std::vector<float> >();
    for (auto &[chunk_id, chunk_vertices]: this->chunk_visible_vertices) {
        vertices->insert(vertices->end(), chunk_vertices.begin(), chunk_vertices.end());
    }
    return vertices;
}

std::vector<int32_t> World::get_visible_chunk_ids(glm::vec3 center_position) {
}

void World::unload_chunks(const std::vector<int32_t> &chunk_ids) {
    for (const auto chunk_id_to_unload: chunk_ids) {
        unload_chunk(chunk_id_to_unload);
    }
}

#ifdef MINECRAFT_DEBUG
void print_chunk_lifetime_info(const glm::vec3 &center_position) {
    const auto center_chunk_id = world_coords_to_chunk_id({
        static_cast<int>(center_position.x),
        static_cast<int>(center_position.y),
        static_cast<int>(center_position.z)
    });

    PRINT_DEBUG("World::check_chunk_lifetimes | id=" << center_chunk_id << " ("
                << "x=" << center_position.x << ", "
                << "y=" << center_position.y << ", "
                << "z=" << center_position.z << ")"
                << std::flush);
}
#endif


void World::check_chunk_lifetimes(glm::vec3 center_position) {
#ifdef MINECRAFT_DEBUG
    print_chunk_lifetime_info(center_position);
#endif

    auto chunks_to_load = std::vector<int32_t>{};
    const auto visible_chunks_ids = WorldGeneration::chunk_ids_around_position(
        center_position,
        WORLD_RENDER_DISTANCE_BLOCKS
    );
    for (const auto chunk_id: visible_chunks_ids) {
        if (this->chunks.contains(chunk_id))
            continue;

        chunks_to_load.push_back(chunk_id);
    }

    std::unordered_set<int32_t> chunks_to_unload{};
    auto pending_loading_chunks = world_generation->load_chunks_async(this->shared_from_this(), chunks_to_load);
    for (auto &[chunk_id, future]: pending_loading_chunks) {
        this->pending_chunks[chunk_id] = std::move(future);
    }

    for (auto &[chunk_id, chunk]: this->chunks) {
        if (!visible_chunks_ids.contains(chunk_id)) {
            chunks_to_unload.insert(chunk_id);
            continue;
        }

        this->chunk_visible_vertices.insert_or_assign(chunk_id, chunk->generate_mesh(false));
    }
}

void World::unload_chunk(const int32_t id) {
    this->chunks.erase(id);
    this->chunk_visible_vertices.erase(id);
    PRINT_DEBUG_IF(WORLD_DEBUG_FLAG, "unloaded chunk=" << id << std::flush);
}

bool World::is_chunk_loaded(int32_t chunk_id) {
    return chunks.find(chunk_id) != chunks.end();
}

std::expected<std::reference_wrapper<Chunk>, ChunkError> World::get_chunk(int32_t chunk_id) {
    const auto it = chunks.find(chunk_id);
    if (it == chunks.end())
        return std::unexpected(ChunkError::NOT_FOUND);

    return *it->second;
}

std::expected<std::reference_wrapper<Chunk>, ChunkError> World::get_chunk(const WorldCoord coords) {
    return get_chunk(world_coords_to_chunk_id(coords));
}