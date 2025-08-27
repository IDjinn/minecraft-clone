//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLDCONSTANTS_H
#define MINECRAFT_WORLDCONSTANTS_H

#define WORLD_SIZE_X 64
#define WORLD_SIZE_Y 64
#define WORLD_SIZE_Z 64

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16


#define WORLD_SPAWN_COORDS glm::vec3(1000, 100, 1000)
#define WORLD_RENDER_DISTANCE_BLOCKS (glm::vec3(5, 2, 5) * (float)CHUNK_SIZE_X)
#define WORLD_RENDER_VERTICES_RESERVE 300000

#define WORLD_TERRAIN_MAX_HEIGHT 120
#define WORLD_TERRAIN_MIN_HEIGHT 1

struct WorldCoord {
    int32_t x, y, z;
};

constexpr int32_t world_coords_to_chunk_id(const WorldCoord coord) {
    const auto chunkX = coord.x / CHUNK_SIZE_X;
    const auto chunkY = coord.y / CHUNK_SIZE_Y;
    const auto chunkZ = coord.z / CHUNK_SIZE_Z;

    return chunkX + (WORLD_SIZE_X * chunkY) + (WORLD_SIZE_X * WORLD_SIZE_Y * chunkZ);
}

constexpr WorldCoord chunk_id_to_world_coordinates(const int32_t id) {
    const auto chunkZ = id / (WORLD_SIZE_X * WORLD_SIZE_Y);
    const auto remaining = id % (WORLD_SIZE_X * WORLD_SIZE_Y);
    const auto chunkY = remaining / WORLD_SIZE_X;
    const auto chunkX = remaining % WORLD_SIZE_X;

    return {chunkX * CHUNK_SIZE_X, chunkY * CHUNK_SIZE_Y, chunkZ * CHUNK_SIZE_Z};
}

constexpr bool isOutOfBounds(const int x, const int y, const int z) {
    return x >= WORLD_SIZE_X || y >= WORLD_SIZE_Y || z >= WORLD_SIZE_Z || x < 0 || y < 0 || z < 0;
}

const float faceVertices[6][30] = {
    // front face (z+) - posição + tex coords
    {
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f
    },
    // back face (z-)
    {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
    },
    // left face (x-)
    {
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f
    },
    // right face (x+)
    {
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f
    },
    // top face (y+)
    {
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f
    },
    // bottom face (y-)
    {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
    }
};

const int directions[6][3] = {
    {0, 0, 1}, // front
    {0, 0, -1}, // back
    {-1, 0, 0}, // left
    {1, 0, 0}, // right
    {0, 1, 0}, // top
    {0, -1, 0} // bottom
};

#define CUBE_FACES 6

#define WORLD_MAX_VERTICES (long long)((long long)(WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z) * \
(long long)(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z) * \
CUBE_FACES * 30l)

#define OVERWORLD 1

#endif //MINECRAFT_WORLDCONSTANTS_H