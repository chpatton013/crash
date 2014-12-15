#include <catch.hpp>
#include <crash/math/math.hpp>

using namespace crash::math;

template < class T >
static void checkAverages(T* lower, T* upper, T* avg, int count) {
   for (int ndx = 0; ndx < count; ++ndx) {
      REQUIRE(average(lower[ndx], upper[ndx]) == avg[ndx]);
   }
}

TEST_CASE("math/average/integer") {
   int lower[] = {0, 1, 2, 3,  4,  5};
   int upper[] = {0, 2, 4, 8, 16, 32};
   int avg[] =   {0, 1, 3, 5, 10, 18};

   checkAverages< int >(lower, upper, avg, 6);
   checkAverages< int >(upper, lower, avg, 6);
}

TEST_CASE("math/average/float") {
   float lower[] = {0.0f, 1.0f, 2.0f, 3.0f,  4.0f,  5.0f};
   float upper[] = {0.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f};
   float avg[] =   {0.0f, 1.5f, 3.0f, 5.5f, 10.0f, 18.5f};

   checkAverages< float >(lower, upper, avg, 6);
   checkAverages< float >(upper, lower, avg, 6);
}
