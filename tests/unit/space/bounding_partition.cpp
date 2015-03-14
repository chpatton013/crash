#include <catch.hpp>
#include <crash/math/symbols.hpp>
#include <crash/math/transformer.hpp>
#include <crash/space/bounding_box.hpp>
#include <crash/space/camera.hpp>
#include <crash/space/bounding_partition.hpp>

using namespace crash::math;
using namespace crash::space;

TEST_CASE("crash/space/bounding_partition/size") {
   glm::vec3 size = glm::vec3(4.0f);
   Transformer transformer(origin, glm::vec4(xAxis, 0.0f), size);
   glm::ivec3 partitions = glm::ivec3(1);
   BoundingPartition partition = BoundingPartition(transformer, partitions);

   REQUIRE(partition.getSize() == size);
   REQUIRE(partition.getPartitions() == partitions);

   size = glm::vec3(16.0f);
   transformer = Transformer(origin, glm::vec4(xAxis, 0.0f), size);
   partitions = glm::ivec3(4);
   partition.resize(transformer, partitions);

   REQUIRE(partition.getSize() == size);
   REQUIRE(partition.getPartitions() == partitions);
}

TEST_CASE("crash/space/bounding_partition/bounding_boxes") {
   glm::vec3 size = glm::vec3(4.0f);
   Transformer transformer(origin, glm::vec4(xAxis, 0.0f), size);
   BoundingPartition partition = BoundingPartition(transformer, glm::ivec3(1));

   const unsigned int numBoundingBoxes = 4;
   for (unsigned int ndx = 0; ndx < numBoundingBoxes; ++ndx) {
      REQUIRE(partition.add(new BoundingBox(Transformer(origin, glm::vec4(xAxis, 0.0f),
       glm::vec3(1.0f)), glm::vec3(), glm::vec4(xAxis, 0.0f))));
   }

   REQUIRE(partition.getNumBoundingGroups() == 1);
   REQUIRE(partition.getBoundingGroups().size() == 1);
   REQUIRE(partition.getNumBoundingBoxes() == numBoundingBoxes);
   REQUIRE(partition.getBoundingBoxes().size() == numBoundingBoxes);

   partition.resize(transformer, glm::ivec3(1));

   REQUIRE(partition.getNumBoundingGroups() == 1);
   REQUIRE(partition.getBoundingGroups().size() == 1);
   REQUIRE(partition.getNumBoundingBoxes() == numBoundingBoxes);
   REQUIRE(partition.getBoundingBoxes().size() == numBoundingBoxes);

   partition.resize(transformer, glm::ivec3(2));

   REQUIRE(partition.getNumBoundingGroups() == 8);
   REQUIRE(partition.getBoundingGroups().size() == 8);
   REQUIRE(partition.getNumBoundingBoxes() == numBoundingBoxes);
   REQUIRE(partition.getBoundingBoxes().size() == numBoundingBoxes);

   std::set< BoundingBox* > partitionBoundingBoxes;
   for (auto boundingBox : partition.getBoundingBoxes()) {
      partitionBoundingBoxes.insert(boundingBox);
   }
   for (auto& group : partition.getBoundingGroups()) {
      std::set< BoundingBox* > groupBoundingBoxes;
      for (auto boundingBox : group.getBoundingBoxes()) {
         groupBoundingBoxes.insert(boundingBox);
      }

      REQUIRE(partitionBoundingBoxes == groupBoundingBoxes);
   }

   for (auto boundingBox : partition.getBoundingBoxes()) {
      REQUIRE(partition.getContainingBoundingGroups(boundingBox).size() == 8);
   }

   for (auto boundingBox : partition.getBoundingBoxes()) {
      delete boundingBox;
   }
}

TEST_CASE("crash/space/bounding_partition/visible_elements") {
   glm::vec3 size = glm::vec3(32.0f);
   Transformer transformer(origin, glm::vec4(xAxis, 0.0f), size);
   BoundingPartition partition = BoundingPartition(transformer, glm::ivec3(1));

   Camera c = Camera(Transformer(origin, glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)),
    /* fov */ glm::radians(60.0f), /* aspect */ 1.0f, /* near */ 3.0f, /* far */ 32.0f);

   std::set< BoundingBox* > inView = {{
      new BoundingBox(Transformer(
         glm::vec3( 0.0f,  0.0f, size.z * 0.5f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3( 0.0f,  0.0f, size.z * 0.5f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3( 1.0f,  1.0f,          5.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3( 1.0f, -1.0f,          5.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(-1.0f,  1.0f,          5.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(-1.0f, -1.0f,          5.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
   }};

   std::set< BoundingBox* > outOfView = {{
      new BoundingBox(Transformer(
         glm::vec3(         0.0f,          0.0f,           0.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(         0.0f,          0.0f, -size.z * 0.05), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(         0.0f, size.y * 0.5f,           0.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(         0.0f, size.y * 0.5f, -size.z * 0.05), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(size.x * 0.5f,          0.0f,           0.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(size.x * 0.5f,          0.0f, -size.z * 0.05), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(size.x * 0.5f, size.y * 0.5f,           0.0f), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
      new BoundingBox(Transformer(
         glm::vec3(size.x * 0.5f, size.y * 0.5f, -size.z * 0.05), glm::vec4(xAxis, 0.0f), glm::vec3(1.0f)
      ), glm::vec3(), glm::vec4(xAxis, 0.0f)),
   }};

   for (auto boundingBox : inView) {
      REQUIRE(partition.add(boundingBox));
   }
   for (auto boundingBox : outOfView) {
      REQUIRE(partition.add(boundingBox));
   }

   std::vector< BoundingBox* > visibleBoundingBoxes = partition.getVisibleElements(c.getViewFrustum());
   for (auto boundingBox : visibleBoundingBoxes) {
      REQUIRE(inView.find(boundingBox) != inView.end());
      REQUIRE(outOfView.find(boundingBox) == outOfView.end());
   }

   for (auto boundingBox : inView) {
      delete boundingBox;
   }
   for (auto boundingBox : outOfView) {
      delete boundingBox;
   }
}
