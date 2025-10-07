//
// Created by Luke on 06/10/2025.
//

#include <iostream>

#include "catch2/catch_test_macros.hpp"
#include "game/world/WorldConstants.h"
#include "glm/vec3.hpp"

TEST_CASE (
"chunk mesh tests"
)
 {
    SECTION("world coords => [chunks]") {
        const auto max_chunks = 16;
        const auto start_point = WORLD_SPAWN_COORDS;

        for (auto y = static_cast<int>(start_point.y); y < max_chunks; y += CHUNK_SIZE_Y) {
            for (auto x = static_cast<int>(start_point.x); x < max_chunks; x += CHUNK_SIZE_X) {
                for (auto z = static_cast<int>(start_point.z); z < max_chunks; z += CHUNK_SIZE_Z) {
                    const auto chunk_id = world_coords_to_chunk_id({x, y, z});
                    auto chunk_coords = chunk_id_to_world_coordinates(chunk_id);
                    REQUIRE(chunk_coords.absolute);

                    const auto expected_chunk_id = world_coords_to_chunk_id(chunk_coords);
                    REQUIRE(chunk_id == expected_chunk_id);

                    SECTION("Chunk mesh check") {

                    }
                }
            }
        }
    }
}