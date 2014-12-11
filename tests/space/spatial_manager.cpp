#include <catch.hpp>
#include <crash/math/math.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/spatial_manager.hpp>

using namespace crash::space;

TEST_CASE("crash/space/spatial_manager/size") {
   glm::vec3 dimensions = glm::vec3(4.0f);
   glm::ivec3 partitions = glm::ivec3(1);
   SpatialManager mgr = SpatialManager(dimensions, partitions);

   REQUIRE(mgr.getDimensions() == dimensions);
   REQUIRE(mgr.getPartitions() == partitions);

   dimensions = glm::vec3(16.0f);
   partitions = glm::ivec3(4);
   mgr.resize(dimensions, partitions);

   REQUIRE(mgr.getDimensions() == dimensions);
   REQUIRE(mgr.getPartitions() == partitions);
}

TEST_CASE("crash/space/spatial_manager/boundables") {
   glm::vec3 dimensions = glm::vec3(4.0f);
   SpatialManager mgr = SpatialManager(dimensions, glm::ivec3(1));

   int numBoundables = 4;
   for (int ndx = 0; ndx < numBoundables; ++ndx) {
      mgr.add(new Boundable(crash::math::average(crash::math::origin, dimensions)));
   }

   REQUIRE(mgr.getBoundingGroupCount() == 1);
   REQUIRE(mgr.getBoundingGroups().size() == 1);
   REQUIRE(mgr.getBoundableCount() == numBoundables);
   REQUIRE(mgr.getBoundables().size() == numBoundables);

   mgr.resize(dimensions, glm::ivec3(2));

   REQUIRE(mgr.getBoundingGroupCount() == 8);
   REQUIRE(mgr.getBoundingGroups().size() == 8);
   REQUIRE(mgr.getBoundableCount() == numBoundables);
   REQUIRE(mgr.getBoundables().size() == numBoundables);

   std::set< Boundable* > mgrBoundables;
   for (auto boundable : mgr.getBoundables()) {
      mgrBoundables.insert(boundable);
   }
   for (auto group : mgr.getBoundingGroups()) {
      std::set< Boundable* > groupBoundables;
      for (auto boundable : group.getBoundables()) {
         groupBoundables.insert(boundable);
      }

      REQUIRE(mgrBoundables == groupBoundables);
   }

   for (auto boundable : mgr.getBoundables()) {
      REQUIRE(mgr.getContainingGroups(boundable).size() == 8);
   }

   for (auto boundable : mgr.getBoundables()) {
      delete boundable;
   }
}

TEST_CASE("crash/space/spatial_manager/render_queue") {
   glm::vec3 dimensions = glm::vec3(32.0f);
   SpatialManager mgr = SpatialManager(dimensions, glm::ivec3(1));

   Camera c = Camera(/* position */ crash::math::origin,
    /* orientation */ glm::vec4(crash::math::xAxis, 0.0f),
    /* fov */ glm::radians(60.0f), /* aspect */ 1.0f,
    /* near */ 3.0f, /* far */ 30.0f);

   std::set< Boundable* > inView = {
      new Boundable(dimensions * 0.5f)
   };

   std::set< Boundable* > outOfView = {{
      new Boundable(glm::vec3(        0.0f,         0.0f,         0.0f)),
      new Boundable(glm::vec3(        0.0f,         0.0f, dimensions.z)),
      new Boundable(glm::vec3(        0.0f, dimensions.y,         0.0f)),
      new Boundable(glm::vec3(        0.0f, dimensions.y, dimensions.z)),
      new Boundable(glm::vec3(dimensions.x,         0.0f,         0.0f)),
      new Boundable(glm::vec3(dimensions.x,         0.0f, dimensions.z)),
      new Boundable(glm::vec3(dimensions.x, dimensions.y,         0.0f)),
      new Boundable(glm::vec3(dimensions.x, dimensions.y, dimensions.z))
   }};

   for (auto boundable : inView) {
      mgr.add(boundable);
   }
   for (auto boundable : outOfView) {
      mgr.add(boundable);
   }

   std::vector< Boundable* > visibleBoundables = mgr.getRenderQueue(c.getViewFrustum());
   for (auto boundable : visibleBoundables) {
      REQUIRE(inView.find(boundable) != inView.end());
      REQUIRE(outOfView.find(boundable) == inView.end());
   }

   for (auto boundable : inView) {
      delete boundable;
   }
   for (auto boundable : outOfView) {
      delete boundable;
   }
}
