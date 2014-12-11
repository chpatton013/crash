#include <catch.hpp>
#include <crash/math/math.hpp>
#include <crash/space/boundable.hpp>
#include <crash/space/spatial_manager.hpp>

using namespace crash::space;

TEST_CASE("crash/space/spatial_manager/size") {
   glm::vec3 size = glm::vec3(4.0f);
   glm::ivec3 partitions = glm::ivec3(1);
   SpatialManager mgr = SpatialManager(size, partitions);

   REQUIRE(mgr.getSize() == size);
   REQUIRE(mgr.getPartitions() == partitions);

   size = glm::vec3(16.0f);
   partitions = glm::ivec3(4);
   mgr.resize(size, partitions);

   REQUIRE(mgr.getSize() == size);
   REQUIRE(mgr.getPartitions() == partitions);
}

TEST_CASE("crash/space/spatial_manager/boundables") {
   glm::vec3 size = glm::vec3(4.0f);
   SpatialManager mgr = SpatialManager(size, glm::ivec3(1));

   const unsigned int numBoundables = 4;
   for (unsigned int ndx = 0; ndx < numBoundables; ++ndx) {
      REQUIRE(mgr.add(new Boundable()));
   }

   REQUIRE(mgr.getBoundingGroupCount() == 1);
   REQUIRE(mgr.getBoundingGroups().size() == 1);
   REQUIRE(mgr.getBoundableCount() == numBoundables);
   REQUIRE(mgr.getBoundables().size() == numBoundables);

   mgr.resize(size, glm::ivec3(1));

   REQUIRE(mgr.getBoundingGroupCount() == 1);
   REQUIRE(mgr.getBoundingGroups().size() == 1);
   REQUIRE(mgr.getBoundableCount() == numBoundables);
   REQUIRE(mgr.getBoundables().size() == numBoundables);

   mgr.resize(size, glm::ivec3(2));

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
   glm::vec3 size = glm::vec3(32.0f);
   SpatialManager mgr = SpatialManager(size, glm::ivec3(1));

   Camera c = Camera(/* position */ crash::math::origin,
    /* orientation */ glm::vec4(crash::math::xAxis, 0.0f),
    /* fov */ glm::radians(60.0f), /* aspect */ 1.0f,
    /* near */ 3.0f, /* far */ 32.0f);

   std::set< Boundable* > inView = {{
      new Boundable(glm::vec3( 0.0f,  0.0f, size.z * 0.5f)),
      new Boundable(glm::vec3( 0.0f,  0.0f, size.z * 0.5f)),
      new Boundable(glm::vec3( 1.0f,  1.0f,          5.0f)),
      new Boundable(glm::vec3( 1.0f, -1.0f,          5.0f)),
      new Boundable(glm::vec3(-1.0f,  1.0f,          5.0f)),
      new Boundable(glm::vec3(-1.0f, -1.0f,          5.0f)),
   }};

   std::set< Boundable* > outOfView = {{
      new Boundable(glm::vec3(         0.0f,          0.0f,           0.0f)),
      new Boundable(glm::vec3(         0.0f,          0.0f, -size.z * 0.05)),
      new Boundable(glm::vec3(         0.0f, size.y * 0.5f,           0.0f)),
      new Boundable(glm::vec3(         0.0f, size.y * 0.5f, -size.z * 0.05)),
      new Boundable(glm::vec3(size.x * 0.5f,          0.0f,           0.0f)),
      new Boundable(glm::vec3(size.x * 0.5f,          0.0f, -size.z * 0.05)),
      new Boundable(glm::vec3(size.x * 0.5f, size.y * 0.5f,           0.0f)),
      new Boundable(glm::vec3(size.x * 0.5f, size.y * 0.5f, -size.z * 0.05)),
   }};

   for (auto boundable : inView) {
      REQUIRE(mgr.add(boundable));
   }
   for (auto boundable : outOfView) {
      REQUIRE(mgr.add(boundable));
   }

   std::vector< Boundable* > visibleBoundables = mgr.getRenderQueue(c.getViewFrustum());
   for (auto boundable : visibleBoundables) {
      REQUIRE(inView.find(boundable) != inView.end());
      REQUIRE(outOfView.find(boundable) == outOfView.end());
   }

   for (auto boundable : inView) {
      delete boundable;
   }
   for (auto boundable : outOfView) {
      delete boundable;
   }
}
