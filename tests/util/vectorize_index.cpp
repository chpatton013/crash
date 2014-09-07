#include <crash/util/util.hpp>
#include <catch.hpp>

using namespace crash::util;

TEST_CASE("util/vectorize_index/linear") {
   glm::ivec3 x = glm::ivec3(2, 1, 1);
   glm::ivec3 y = glm::ivec3(1, 2, 1);
   glm::ivec3 z = glm::ivec3(1, 1, 2);

   REQUIRE(vectorize_index(0, x) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, x) == glm::ivec3(1, 0, 0));

   REQUIRE(vectorize_index(0, y) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, y) == glm::ivec3(0, 1, 0));

   REQUIRE(vectorize_index(0, z) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, z) == glm::ivec3(0, 0, 1));
}

TEST_CASE("util/vectorize_index/square") {
   glm::ivec3 xy = glm::ivec3(2, 2, 1);
   glm::ivec3 xz = glm::ivec3(2, 1, 2);
   glm::ivec3 yz = glm::ivec3(1, 2, 2);

   REQUIRE(vectorize_index(0, xy) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, xy) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, xy) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(3, xy) == glm::ivec3(1, 1, 0));

   REQUIRE(vectorize_index(0, xz) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, xz) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, xz) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(3, xz) == glm::ivec3(1, 0, 1));

   REQUIRE(vectorize_index(0, yz) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, yz) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(2, yz) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(3, yz) == glm::ivec3(0, 1, 1));
}

TEST_CASE("util/vectorize_index/cube") {
   glm::ivec3 bounds = glm::ivec3(2, 2, 2);

   REQUIRE(vectorize_index(0, bounds) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, bounds) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, bounds) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(3, bounds) == glm::ivec3(1, 1, 0));
   REQUIRE(vectorize_index(4, bounds) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(5, bounds) == glm::ivec3(1, 0, 1));
   REQUIRE(vectorize_index(6, bounds) == glm::ivec3(0, 1, 1));
   REQUIRE(vectorize_index(7, bounds) == glm::ivec3(1, 1, 1));
}

TEST_CASE("util/vectorize_index/increasing") {
   glm::ivec3 bounds = glm::ivec3(2, 3, 4);

   REQUIRE(vectorize_index(0, bounds) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, bounds) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, bounds) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(3, bounds) == glm::ivec3(1, 1, 0));
   REQUIRE(vectorize_index(4, bounds) == glm::ivec3(0, 2, 0));
   REQUIRE(vectorize_index(5, bounds) == glm::ivec3(1, 2, 0));
   REQUIRE(vectorize_index(6, bounds) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(7, bounds) == glm::ivec3(1, 0, 1));
   REQUIRE(vectorize_index(8, bounds) == glm::ivec3(0, 1, 1));
   REQUIRE(vectorize_index(9, bounds) == glm::ivec3(1, 1, 1));
   REQUIRE(vectorize_index(10, bounds) == glm::ivec3(0, 2, 1));
   REQUIRE(vectorize_index(11, bounds) == glm::ivec3(1, 2, 1));
   REQUIRE(vectorize_index(12, bounds) == glm::ivec3(0, 0, 2));
   REQUIRE(vectorize_index(13, bounds) == glm::ivec3(1, 0, 2));
   REQUIRE(vectorize_index(14, bounds) == glm::ivec3(0, 1, 2));
   REQUIRE(vectorize_index(15, bounds) == glm::ivec3(1, 1, 2));
   REQUIRE(vectorize_index(16, bounds) == glm::ivec3(0, 2, 2));
   REQUIRE(vectorize_index(17, bounds) == glm::ivec3(1, 2, 2));
   REQUIRE(vectorize_index(18, bounds) == glm::ivec3(0, 0, 3));
   REQUIRE(vectorize_index(19, bounds) == glm::ivec3(1, 0, 3));
   REQUIRE(vectorize_index(20, bounds) == glm::ivec3(0, 1, 3));
   REQUIRE(vectorize_index(21, bounds) == glm::ivec3(1, 1, 3));
   REQUIRE(vectorize_index(22, bounds) == glm::ivec3(0, 2, 3));
   REQUIRE(vectorize_index(23, bounds) == glm::ivec3(1, 2, 3));
}

TEST_CASE("util/vectorize_index/decreasing") {
   glm::ivec3 bounds = glm::ivec3(4, 3, 2);

   REQUIRE(vectorize_index(0, bounds) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, bounds) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, bounds) == glm::ivec3(2, 0, 0));
   REQUIRE(vectorize_index(3, bounds) == glm::ivec3(3, 0, 0));
   REQUIRE(vectorize_index(4, bounds) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(5, bounds) == glm::ivec3(1, 1, 0));
   REQUIRE(vectorize_index(6, bounds) == glm::ivec3(2, 1, 0));
   REQUIRE(vectorize_index(7, bounds) == glm::ivec3(3, 1, 0));
   REQUIRE(vectorize_index(8, bounds) == glm::ivec3(0, 2, 0));
   REQUIRE(vectorize_index(9, bounds) == glm::ivec3(1, 2, 0));
   REQUIRE(vectorize_index(10, bounds) == glm::ivec3(2, 2, 0));
   REQUIRE(vectorize_index(11, bounds) == glm::ivec3(3, 2, 0));
   REQUIRE(vectorize_index(12, bounds) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(13, bounds) == glm::ivec3(1, 0, 1));
   REQUIRE(vectorize_index(14, bounds) == glm::ivec3(2, 0, 1));
   REQUIRE(vectorize_index(15, bounds) == glm::ivec3(3, 0, 1));
   REQUIRE(vectorize_index(16, bounds) == glm::ivec3(0, 1, 1));
   REQUIRE(vectorize_index(17, bounds) == glm::ivec3(1, 1, 1));
   REQUIRE(vectorize_index(18, bounds) == glm::ivec3(2, 1, 1));
   REQUIRE(vectorize_index(19, bounds) == glm::ivec3(3, 1, 1));
   REQUIRE(vectorize_index(20, bounds) == glm::ivec3(0, 2, 1));
   REQUIRE(vectorize_index(21, bounds) == glm::ivec3(1, 2, 1));
   REQUIRE(vectorize_index(22, bounds) == glm::ivec3(2, 2, 1));
   REQUIRE(vectorize_index(23, bounds) == glm::ivec3(3, 2, 1));
}

TEST_CASE("util/vectorize_index/asymmetric") {
   glm::ivec3 bounds = glm::ivec3(2, 4, 3);

   REQUIRE(vectorize_index(0, bounds) == glm::ivec3(0, 0, 0));
   REQUIRE(vectorize_index(1, bounds) == glm::ivec3(1, 0, 0));
   REQUIRE(vectorize_index(2, bounds) == glm::ivec3(0, 1, 0));
   REQUIRE(vectorize_index(3, bounds) == glm::ivec3(1, 1, 0));
   REQUIRE(vectorize_index(4, bounds) == glm::ivec3(0, 2, 0));
   REQUIRE(vectorize_index(5, bounds) == glm::ivec3(1, 2, 0));
   REQUIRE(vectorize_index(6, bounds) == glm::ivec3(0, 3, 0));
   REQUIRE(vectorize_index(7, bounds) == glm::ivec3(1, 3, 0));
   REQUIRE(vectorize_index(8, bounds) == glm::ivec3(0, 0, 1));
   REQUIRE(vectorize_index(9, bounds) == glm::ivec3(1, 0, 1));
   REQUIRE(vectorize_index(10, bounds) == glm::ivec3(0, 1, 1));
   REQUIRE(vectorize_index(11, bounds) == glm::ivec3(1, 1, 1));
   REQUIRE(vectorize_index(12, bounds) == glm::ivec3(0, 2, 1));
   REQUIRE(vectorize_index(13, bounds) == glm::ivec3(1, 2, 1));
   REQUIRE(vectorize_index(14, bounds) == glm::ivec3(0, 3, 1));
   REQUIRE(vectorize_index(15, bounds) == glm::ivec3(1, 3, 1));
   REQUIRE(vectorize_index(16, bounds) == glm::ivec3(0, 0, 2));
   REQUIRE(vectorize_index(17, bounds) == glm::ivec3(1, 0, 2));
   REQUIRE(vectorize_index(18, bounds) == glm::ivec3(0, 1, 2));
   REQUIRE(vectorize_index(19, bounds) == glm::ivec3(1, 1, 2));
   REQUIRE(vectorize_index(20, bounds) == glm::ivec3(0, 2, 2));
   REQUIRE(vectorize_index(21, bounds) == glm::ivec3(1, 2, 2));
   REQUIRE(vectorize_index(22, bounds) == glm::ivec3(0, 3, 2));
   REQUIRE(vectorize_index(23, bounds) == glm::ivec3(1, 3, 2));
}
