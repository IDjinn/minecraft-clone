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

std::unique_ptr<Chunk> WorldGeneration::load_chunk(const std::weak_ptr<World> &world, int32_t chunk_id) {
    auto chunk = std::make_unique<Chunk>(chunk_id, world);
    auto [chunkX, chunkY, chunkZ, absolute] = chunk_id_to_world_coordinates(chunk_id);
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
    return chunk;
}

std::unique_ptr<std::unordered_map<int32_t, std::unique_ptr<Chunk> > >
WorldGeneration::generate_chunks_around(
    const std::weak_ptr<World> &world,
    glm::vec3 position
) {
    const auto world_min_boundary = position - WORLD_RENDER_DISTANCE_BLOCKS;
    const auto world_max_boundary = position + WORLD_RENDER_DISTANCE_BLOCKS;

    const auto minX = std::max(static_cast<int>(world_min_boundary.x), 0);
    const auto minY = std::max(static_cast<int>(world_min_boundary.y), 0);
    const auto minZ = std::max(static_cast<int>(world_min_boundary.z), 0);

    const auto maxX = static_cast<int>(world_max_boundary.x);
    const auto maxY = static_cast<int>(world_max_boundary.y);
    const auto maxZ = static_cast<int>(world_max_boundary.z);
    auto chunks = std::unordered_map<int32_t, std::unique_ptr<Chunk> >{};

    for (auto y = minY; y < maxY; y += CHUNK_SIZE_Y) {
        for (auto x = minX; x < maxX; x += CHUNK_SIZE_X) {
            for (auto z = minZ; z < maxZ; z += CHUNK_SIZE_Z) {
                const auto chunk_id = world_coords_to_chunk_id({x, y, z});
                chunks[chunk_id] = load_chunk(world, chunk_id);

#if MINECRAFT_DEBUG
                auto [world_x, world_y, world_z, absolute] = chunk_id_to_world_coordinates(chunk_id);
                PRINT_DEBUG_IF(WORLD_DEBUG_FLAG,
                               "loaded chunk=" << chunk_id <<" at x=" << x << " y=" << y << " z=" << z << " (" <<
                               world_x <<
                               ", "
                               << world_y << ", " << world_z<<" a="<<absolute<<")"
                );
                WHEN_DEBUG(std::cout << std::flush);
#endif
            }
        }
    }
    return std::make_unique<std::unordered_map<int32_t, std::unique_ptr<Chunk> > >(std::move(chunks));
}
