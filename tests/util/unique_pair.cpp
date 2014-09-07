#include <tuple>
#include <crash/util/util.hpp>
#include <catch.hpp>

using namespace crash::util;

static void fillUniquePairs(std::set< std::tuple< int, int > >& set, int count) {
   for (int ndx1 = 0; ndx1 < count; ++ndx1) {
      for (int ndx2 = 0; ndx2 < count; ++ndx2) {
         set.insert(unique_pair< int >(ndx1, ndx2));
      }
   }
}

TEST_CASE("util/unique_pair/sanity_check") {
   std::set< std::tuple< int, int > > set;
   std::tuple< int, int > pair1 = unique_pair< int >(0, 0);
   std::tuple< int, int > pair2 = unique_pair< int >(0, 0);

   set.insert(pair1);
   set.insert(pair2);

   REQUIRE(pair1 == pair2);
   REQUIRE(set.size() == 1);
}

TEST_CASE("util/unique_pair/small") {
   std::set< std::tuple< int, int > > set;

   fillUniquePairs(set, 2);
   REQUIRE(set.size() == 3);

   for (auto pair : set) {
      REQUIRE(std::get<0>(pair) <= std::get<1>(pair));
   }
}

TEST_CASE("util/unique_pair/large") {
   std::set< std::tuple< int, int > > set;

   fillUniquePairs(set, 10);
   REQUIRE(set.size() == 55);

   for (auto pair : set) {
      REQUIRE(std::get<0>(pair) <= std::get<1>(pair));
   }
}
