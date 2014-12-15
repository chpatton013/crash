#include <catch.hpp>
#include <glm/glm.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

TEST_CASE("crash/window/mutators/title") {
   Window window(glm::ivec2(400, 300));

   REQUIRE(window.title() == "");
   window.title("title");
   REQUIRE(window.title() == "title");
}

TEST_CASE("crash/window/mutators/position") {
   Window window(glm::ivec2(400, 300));

   window.position(glm::ivec2());
   REQUIRE(window.position() == glm::ivec2());

   window.position(glm::ivec2(500, 500));
   REQUIRE(window.position() == glm::ivec2(500, 500));
}

TEST_CASE("crash/window/mutators/minimized") {
   Window window(glm::ivec2(400, 300));

   window.minimized(false);
   REQUIRE(window.minimized() == false);

   window.minimized(true);
   REQUIRE(window.minimized() == true);
}

TEST_CASE("crash/window/mutators/should-close") {
   Window window(glm::ivec2(400, 300));

   window.shouldClose(false);
   REQUIRE(window.shouldClose() == false);

   window.shouldClose(true);
   REQUIRE(window.shouldClose() == true);
}

TEST_CASE("crash/window/mutators/user-pointer") {
   Window window(glm::ivec2(400, 300));

   const char* userPointer = "user pointer data";

   REQUIRE(window.userPointer() == nullptr);
   window.userPointer((void*)userPointer);
   REQUIRE(window.userPointer() == userPointer);
}
