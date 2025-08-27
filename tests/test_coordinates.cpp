//
// Created by Luke on 27/08/2025.
//
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "game/world/WorldConstants.h"

TEST_CASE("test world coordinates", "[world]") {
    SECTION("world xyz => chunk id") {
        auto coords = WorldCoord{1234, 1023, 21343};
        auto chunk_id = world_coords_to_chunk_id(coords);
        auto chunk_coords = chunk_id_to_world_coordinates(chunk_id);
        REQUIRE(chunk_coords.absolute);

        auto expected_chunk_id = world_coords_to_chunk_id(chunk_coords);
        REQUIRE(chunk_id == expected_chunk_id);
    }

    SECTION("world xyz const => chunk id const") {
        auto coords = WorldCoord{1000, 100, 1000};
        auto chunk_id = world_coords_to_chunk_id(coords);
        int32_t expected_chunk_id = 254398;
        REQUIRE(chunk_id == expected_chunk_id);
    }

    SECTION("chunk const => world xyz const") {
        int32_t chunk_id = 254398;
        auto coords = chunk_id_to_world_coordinates(chunk_id);
        auto expected_coords = WorldCoord{1000, 100, 1000, false};
        REQUIRE(expected_coords.soft_equals(coords));
    }

    SECTION("world coords => [absolute]") {
        auto left = WorldCoord{1000, 100, 1000, true};
        auto right = WorldCoord{992, 100, 1000, false};
        REQUIRE(left != right);
    }

    SECTION("world coords => [relative]") {
        auto left = WorldCoord{1000, 100, 1000, false};
        auto right = WorldCoord{992, 100, 1000, false};
        REQUIRE(left == right);
    }
}
