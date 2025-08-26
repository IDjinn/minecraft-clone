//
// Created by Luke on 07/08/2025.
//

#include "WorldGenerator.h"

std::unique_ptr<World> WorldGenerator::generateWorld() {
    auto world = std::make_unique<World>(OVERWORLD, WORLD_SPAWN_COORDS);

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
                auto chunkLoaded = world->loadChunk(chunk_id);
                ASSERT_DEBUG(chunkLoaded, "failed loading chunk");
                if (!chunkLoaded) continue;

                auto [world_x, world_y, world_z] = World::chunk_id_to_world_coordinates(chunk_id);
                PRINT_DEBUG(
                    "loaded chunk=" << chunk_id <<" at x=" << x << " y=" << y << " z=" << z << " (" << world_x << ", "
                    << world_y << ", " << world_z<<")");
                WHEN_DEBUG(std::cout << std::flush);

                auto &chunk = world->getChunk(chunk_id);
                chunk.setIndex(chunk_id);
                chunk.initializeBlocks();
            }
        }
    }

    return world;
}
