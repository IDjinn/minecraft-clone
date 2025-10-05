//
// Created by Luke on 27/08/2025.
//

#include "WorldGeneration.h"

#include "../WorldConstants.h"
#include "../World.h"
#include "../chunks/Chunk.h"


WorldGeneration::WorldGeneration(const long seed) : seed(seed) {
    fnGenerator = FastNoise::New<FastNoise::Simplex>();
    heightMap = std::vector<float>(CHUNK_SIZE_X * CHUNK_SIZE_Y);
}

void WorldGeneration::load_chunk(const std::unique_ptr<Chunk> &chunk) {
    auto [chunkX, chunkY, chunkZ, absolute] = chunk_id_to_world_coordinates(chunk->id);
    fnGenerator->GenUniformGrid2D(
        heightMap.data(),
        chunkX * CHUNK_SIZE_X,
        chunkZ * CHUNK_SIZE_Z,
        CHUNK_SIZE_X,
        CHUNK_SIZE_Z,
        0.007f,
        seed
    );

    for (auto x = 0; x < CHUNK_SIZE_X; x++) {
        for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
            const auto heightIndex = x + z * CHUNK_SIZE_X;
            const auto noiseValue = heightMap[heightIndex];
            auto terrainHeight = static_cast<int>(
                (noiseValue + 1.0f) * 0.5f * WORLD_TERRAIN_MAX_HEIGHT + WORLD_TERRAIN_MIN_HEIGHT);

            terrainHeight = std::max(0, std::min(CHUNK_SIZE_Y - 1, terrainHeight));
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                const auto index = Chunk::block_index(x, y, z);
                chunk->blocks[index].setIndex(index);

                if (y <= terrainHeight) {
                    // if (y == terrainHeight) {
                    chunk->blocks[index].setBlockType(BlockType::DIRT);
                    // } else if (y >= terrainHeight - 3) {
                    // chunk->blocks[index].setBlockType(BlockType::DIRT);
                    // } else {
                    // chunk->blocks[index].setBlockType(BlockType::STONE);
                    // }
                } else {
                    chunk->blocks[index].setBlockType(BlockType::AIR);
                }
            }
        }
    }

    chunk->set_state(ChunkState::INITIALIZED);
}

std::unordered_set<int32_t> WorldGeneration::chunks_around(const glm::vec3 position, const glm::vec3 render_distance) {
    const auto world_min_boundary = position - render_distance;
    const auto world_max_boundary = position + render_distance;

    const auto minX = std::max(static_cast<int>(world_min_boundary.x), 0);
    const auto minY = std::max(static_cast<int>(world_min_boundary.y), 0);
    const auto minZ = std::max(static_cast<int>(world_min_boundary.z), 0);

    const auto maxX = static_cast<int>(world_max_boundary.x);
    const auto maxY = static_cast<int>(world_max_boundary.y);
    const auto maxZ = static_cast<int>(world_max_boundary.z);
    std::unordered_set<int32_t> chunks{};

    for (auto y = minY; y < maxY; y += CHUNK_SIZE_Y) {
        for (auto x = minX; x < maxX; x += CHUNK_SIZE_X) {
            for (auto z = minZ; z < maxZ; z += CHUNK_SIZE_Z) {
                chunks.emplace(world_coords_to_chunk_id({x, y, z}));
            }
        }
    }

    return chunks;
}

std::unordered_map<int32_t, std::unique_ptr<Chunk> >
WorldGeneration::load_chunks(
    const std::shared_ptr<World> &world,
    const std::vector<int32_t> &chunk_ids
) {
    std::unordered_map<int32_t, std::unique_ptr<Chunk> > chunks{};
    for (const auto chunk_id: chunk_ids) {
        chunks[chunk_id] = std::make_unique<Chunk>(chunk_id, world);
        load_chunk(chunks[chunk_id]);

#if MINECRAFT_DEBUG
        auto [world_x, world_y, world_z, absolute] = chunk_id_to_world_coordinates(chunk_id);
        PRINT_DEBUG_IF(WORLD_DEBUG_FLAG,
                       "loaded chunk=" << chunk_id <<" (" <<world_x << ", " << world_y << ", " << world_z<<" a="<<
                       absolute<<")" << std::flush);
#endif
    }

    return chunks;
}
