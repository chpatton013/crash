#include <catch.hpp>
#include <glm/glm.hpp>
#include <crash/window/monitor.hpp>
#include <crash/window/window.hpp>

using namespace crash::window;

TEST_CASE("crash/window/mutators/title") {
   Window window(glm::ivec2(400, 300), "", boost::none, boost::none);

   REQUIRE(window.getTitle() == "");
   window.setTitle("title");
   REQUIRE(window.getTitle() == "title");
}

TEST_CASE("crash/window/mutators/position") {
   Window window(glm::ivec2(400, 300), "", boost::none, boost::none);

   window.setWindowPosition(glm::ivec2());
   REQUIRE(window.getWindowPosition() == glm::ivec2());

   window.setWindowPosition(glm::ivec2(500, 500));
   REQUIRE(window.getWindowPosition() == glm::ivec2(500, 500));
}

TEST_CASE("crash/window/mutators/minimized") {
   Window window(glm::ivec2(400, 300), "", boost::none, boost::none);

   window.setMinimized(false);
   REQUIRE(window.isMinimized() == false);

   window.setMinimized(true);
   REQUIRE(window.isMinimized() == true);
}

TEST_CASE("crash/window/mutators/should-close") {
   Window window(glm::ivec2(400, 300), "", boost::none, boost::none);

   window.setShouldClose(false);
   REQUIRE(window.shouldClose() == false);

   window.setShouldClose(true);
   REQUIRE(window.shouldClose() == true);
}

TEST_CASE("crash/window/mutators/user-pointer") {
   Window window(glm::ivec2(400, 300), "", boost::none, boost::none);

   const char* userPointer = "user pointer data";

   REQUIRE(window.getUserPointer() == nullptr);
   window.setUserPointer((void*)userPointer);
   REQUIRE(window.getUserPointer() == userPointer);
}
