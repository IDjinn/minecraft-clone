//
// Created by Luke on 27/08/2025.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <iostream>

TEST_CASE("test setup catch2", "[catch2]") {
    std::cout << "hello world!\n";
    REQUIRE(1 + 1 == 2);
}