#include <catch.hpp>
#include <crash/math/math.hpp>

using namespace crash::math;

TEST_CASE("util/linearize_index/linear") {
   glm::ivec3 x = glm::ivec3(2, 1, 1);
   glm::ivec3 y = glm::ivec3(1, 2, 1);
   glm::ivec3 z = glm::ivec3(1, 1, 2);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), x) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), x) == 1);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), y) == 0);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), y) == 1);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), z) == 0);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), z) == 1);
}

TEST_CASE("util/linearize_index/square") {
   glm::ivec3 xy = glm::ivec3(2, 2, 1);
   glm::ivec3 xz = glm::ivec3(2, 1, 2);
   glm::ivec3 yz = glm::ivec3(1, 2, 2);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), xy) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), xy) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), xy) == 2);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 0), xy) == 3);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), xz) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), xz) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), xz) == 2);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 1), xz) == 3);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), yz) == 0);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), yz) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), yz) == 2);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 1), yz) == 3);
}

TEST_CASE("util/linearize_index/cube") {
   glm::ivec3 bounds = glm::ivec3(2, 2, 2);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), bounds) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), bounds) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), bounds) == 2);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 0), bounds) == 3);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), bounds) == 4);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 1), bounds) == 5);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 1), bounds) == 6);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 1), bounds) == 7);
}

TEST_CASE("util/linearize_index/increasing") {
   glm::ivec3 bounds = glm::ivec3(2, 3, 4);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), bounds) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), bounds) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), bounds) == 2);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 0), bounds) == 3);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 0), bounds) == 4);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 0), bounds) == 5);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), bounds) == 6);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 1), bounds) == 7);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 1), bounds) == 8);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 1), bounds) == 9);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 1), bounds) == 10);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 1), bounds) == 11);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 2), bounds) == 12);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 2), bounds) == 13);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 2), bounds) == 14);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 2), bounds) == 15);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 2), bounds) == 16);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 2), bounds) == 17);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 3), bounds) == 18);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 3), bounds) == 19);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 3), bounds) == 20);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 3), bounds) == 21);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 3), bounds) == 22);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 3), bounds) == 23);
}

TEST_CASE("util/linearize_index/decreasing") {
   glm::ivec3 bounds = glm::ivec3(4, 3, 2);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), bounds) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), bounds) == 1);
   REQUIRE(linearize_index(glm::ivec3(2, 0, 0), bounds) == 2);
   REQUIRE(linearize_index(glm::ivec3(3, 0, 0), bounds) == 3);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), bounds) == 4);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 0), bounds) == 5);
   REQUIRE(linearize_index(glm::ivec3(2, 1, 0), bounds) == 6);
   REQUIRE(linearize_index(glm::ivec3(3, 1, 0), bounds) == 7);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 0), bounds) == 8);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 0), bounds) == 9);
   REQUIRE(linearize_index(glm::ivec3(2, 2, 0), bounds) == 10);
   REQUIRE(linearize_index(glm::ivec3(3, 2, 0), bounds) == 11);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), bounds) == 12);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 1), bounds) == 13);
   REQUIRE(linearize_index(glm::ivec3(2, 0, 1), bounds) == 14);
   REQUIRE(linearize_index(glm::ivec3(3, 0, 1), bounds) == 15);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 1), bounds) == 16);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 1), bounds) == 17);
   REQUIRE(linearize_index(glm::ivec3(2, 1, 1), bounds) == 18);
   REQUIRE(linearize_index(glm::ivec3(3, 1, 1), bounds) == 19);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 1), bounds) == 20);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 1), bounds) == 21);
   REQUIRE(linearize_index(glm::ivec3(2, 2, 1), bounds) == 22);
   REQUIRE(linearize_index(glm::ivec3(3, 2, 1), bounds) == 23);
}

TEST_CASE("util/linearize_index/asymmetric") {
   glm::ivec3 bounds = glm::ivec3(2, 4, 3);

   REQUIRE(linearize_index(glm::ivec3(0, 0, 0), bounds) == 0);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 0), bounds) == 1);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 0), bounds) == 2);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 0), bounds) == 3);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 0), bounds) == 4);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 0), bounds) == 5);
   REQUIRE(linearize_index(glm::ivec3(0, 3, 0), bounds) == 6);
   REQUIRE(linearize_index(glm::ivec3(1, 3, 0), bounds) == 7);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 1), bounds) == 8);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 1), bounds) == 9);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 1), bounds) == 10);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 1), bounds) == 11);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 1), bounds) == 12);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 1), bounds) == 13);
   REQUIRE(linearize_index(glm::ivec3(0, 3, 1), bounds) == 14);
   REQUIRE(linearize_index(glm::ivec3(1, 3, 1), bounds) == 15);
   REQUIRE(linearize_index(glm::ivec3(0, 0, 2), bounds) == 16);
   REQUIRE(linearize_index(glm::ivec3(1, 0, 2), bounds) == 17);
   REQUIRE(linearize_index(glm::ivec3(0, 1, 2), bounds) == 18);
   REQUIRE(linearize_index(glm::ivec3(1, 1, 2), bounds) == 19);
   REQUIRE(linearize_index(glm::ivec3(0, 2, 2), bounds) == 20);
   REQUIRE(linearize_index(glm::ivec3(1, 2, 2), bounds) == 21);
   REQUIRE(linearize_index(glm::ivec3(0, 3, 2), bounds) == 22);
   REQUIRE(linearize_index(glm::ivec3(1, 3, 2), bounds) == 23);
}
