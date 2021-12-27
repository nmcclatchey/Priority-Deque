#include "../interval_heap.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cstdlib>

int kArrHeap [] = { 0, 19, 2, 19, 15, 16, 4, 5, 7 };

BOOST_AUTO_TEST_CASE( interval_heap_is_heap )
{
  using namespace boost::heap;
  std::vector<int> empty_arr = std::vector<int>();
  BOOST_TEST_REQUIRE(is_interval_heap(empty_arr.begin(), empty_arr.end(), std::less<int>()));

  std::vector<int> heap_arr;
  heap_arr.insert(heap_arr.end(), kArrHeap, kArrHeap + (sizeof(kArrHeap) / sizeof(kArrHeap[0])));
  BOOST_TEST_REQUIRE(is_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>()));

  heap_arr[1] = 16;
  BOOST_TEST_REQUIRE(!is_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>()));
}

BOOST_AUTO_TEST_CASE( interval_heap_is_until )
{
  using namespace boost::heap;
  std::vector<int> heap_arr;
  heap_arr.insert(heap_arr.end(), kArrHeap, kArrHeap + (sizeof(kArrHeap) / sizeof(kArrHeap[0])));
  BOOST_TEST_REQUIRE((is_interval_heap_until(heap_arr.begin(), heap_arr.end(), std::less<int>()) == heap_arr.end()));

  heap_arr.push_back(21);
  BOOST_TEST_REQUIRE((is_interval_heap_until(heap_arr.begin(), heap_arr.end(), std::less<int>()) == heap_arr.end() - 1));

  heap_arr[5] = -1;
  BOOST_TEST_REQUIRE((is_interval_heap_until(heap_arr.begin(), heap_arr.end(), std::less<int>()) == heap_arr.begin() + 5));

  heap_arr[3] = 1;
  BOOST_TEST_REQUIRE((is_interval_heap_until(heap_arr.begin(), heap_arr.end(), std::less<int>()) == heap_arr.begin() + 3));

  heap_arr.clear();
  BOOST_TEST_REQUIRE((is_interval_heap_until(heap_arr.begin(), heap_arr.end(), std::less<int>()) == heap_arr.end()));
}

BOOST_AUTO_TEST_CASE( interval_heap_pop_max )
{
  using namespace boost::heap;
  std::vector<int> heap_arr;
  //  Test empty (NOP)
  pop_interval_heap_max(heap_arr.begin(), heap_arr.end(), std::less<int>());
  heap_arr.insert(heap_arr.end(), kArrHeap, kArrHeap + 1);
  //  Test 1 element (NOP)
  pop_interval_heap_max(heap_arr.begin(), heap_arr.end(), std::less<int>());
  heap_arr.insert(heap_arr.end(), kArrHeap + 1, kArrHeap + 2);
  //  Test 2 elements (NOP)
  make_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>());
  pop_interval_heap_max(heap_arr.begin(), heap_arr.end(), std::less<int>());
  //  Must be unchanged.
  BOOST_TEST_REQUIRE((heap_arr.front() == 0));

  heap_arr.insert(heap_arr.end(), kArrHeap + 4, kArrHeap + 9);
  //  Test 5 elements
  make_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>());
  pop_interval_heap_max(heap_arr.begin(), heap_arr.end(), std::less<int>());
  BOOST_TEST_REQUIRE((heap_arr.front() == 0));
  BOOST_TEST_REQUIRE((heap_arr[1] == 16));
}

BOOST_AUTO_TEST_CASE( interval_heap_sorting )
{
  using namespace boost::heap;
  std::vector<int> heap_arr;
  for (int i = 0; i < 1051; ++i)
    heap_arr.push_back(i);
  std::vector<int> original = heap_arr;
  make_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>());

  sort_interval_heap(heap_arr.begin(), heap_arr.end(), std::less<int>());
  std::sort(original.begin(), original.end(), std::less<int>());
  BOOST_TEST_REQUIRE(std::equal(heap_arr.begin(), heap_arr.end(), original.begin()));
}

