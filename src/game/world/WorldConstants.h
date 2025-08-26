//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLDCONSTANTS_H
#define MINECRAFT_WORLDCONSTANTS_H

#include <glad/glad.h>

#define WORLD_SIZE_X 64l
#define WORLD_SIZE_Y 64l
#define WORLD_SIZE_Z 64l

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

#define WORLD_SPAWN_COORDS glm::vec3(1000, 1000, 20)
#define WORLD_SPAWN_RENDER_CHUNKS 16


#define CUBE_FACES 6

#define WORLD_MAX_VERTICES (long long)((long long)(WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z) * \
(long long)(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z) * \
CUBE_FACES * 30l)

#define OVERWORLD 0

#endif //MINECRAFT_WORLDCONSTANTS_H