#include "../priority_deque.hpp"

#include <boost/test/unit_test.hpp>

#include <iosfwd>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <random>
#include <set>

namespace
{
template<class T, class T2>
bool have_same_elements(T const & values, std::multiset<T2> rf)
{
  for (T2 const & val : values)
  {
    auto set_it = rf.find(val);
    if (set_it == rf.end())
      return false;
    else
      rf.erase(set_it);
  }
  return rf.empty();
}
}

BOOST_AUTO_TEST_CASE( priority_deque_empty_size )
{
  using namespace boost::container;
  priority_deque<int> pd;
  BOOST_TEST_REQUIRE(pd.empty());
  for (int push_count = 1; push_count < 8; ++push_count)
  {
    for (int i = 0; i < push_count; ++i)
    {
      BOOST_TEST_REQUIRE(pd.size() == i);
      pd.push(rand());
      BOOST_TEST_REQUIRE(!pd.empty());
    }

    BOOST_TEST_REQUIRE(pd.max_size() >= pd.size());

    for (int i = push_count; i; --i)
    {
      BOOST_TEST_REQUIRE(!pd.empty());
      BOOST_TEST_REQUIRE(pd.size() == i);
      pd.pop();
    }
    BOOST_TEST_REQUIRE(pd.empty());
    BOOST_TEST_REQUIRE(pd.size() == 0);
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_iterator )
{
  using namespace boost::container;
  for (int push_count = 1; push_count < 8; ++push_count)
  {
    priority_deque<int> pd;
    BOOST_TEST_REQUIRE((pd.begin() == pd.end()));

    std::multiset<int> existing_elements;
    for (int i = 0; i < push_count; ++i)
    {
      int pushed = rand();
      existing_elements.emplace(pushed);
      pd.push(pushed);
    }

    BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_range_constructor_clear )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 512; ++i)
    existing_elements.emplace(rand());
  priority_deque<int> pd { existing_elements.begin(), existing_elements.end() };

  BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
  pd.clear();
  BOOST_TEST_REQUIRE(pd.empty());
}

BOOST_AUTO_TEST_CASE( priority_deque_emplace )
{
  using namespace boost::container;

  priority_deque<int> pd;
  std::multiset<int> existing_elements;
  for (int i = 0; i < 512; ++i)
  {
    int newval = rand();
    existing_elements.emplace(newval);
    pd.emplace(newval);
  }

  BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
}

BOOST_AUTO_TEST_CASE( priority_deque_insert )
{
  using namespace boost::container;

  std::multiset<int> existing_elements, new_vals;
  for (int i = 0; i < 517; ++i)
    existing_elements.emplace(rand());
  for (int j = 0; j < 139; ++j)
    new_vals.emplace(rand());
  priority_deque<int> pd;
  pd.insert(existing_elements.begin(), existing_elements.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  pd.insert(new_vals.begin(), new_vals.end());
  existing_elements.insert(new_vals.begin(), new_vals.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
}

BOOST_AUTO_TEST_CASE( priority_deque_insert_few )
{
  using namespace boost::container;

  std::multiset<int> existing_elements, new_vals;
  for (int i = 0; i < 1; ++i)
    existing_elements.emplace(rand());
  for (int j = 0; j < 1; ++j)
    new_vals.emplace(rand());
  priority_deque<int> pd;
  pd.insert(existing_elements.begin(), existing_elements.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  pd.insert(new_vals.begin(), new_vals.end());
  existing_elements.insert(new_vals.begin(), new_vals.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
}

BOOST_AUTO_TEST_CASE( priority_deque_min_max )
{
  using namespace boost::container;
  for (int push_count = 1; push_count < 26; ++push_count)
  {
    priority_deque<int> pd;
    BOOST_TEST_REQUIRE((pd.begin() == pd.end()));

    std::multiset<int> existing_elements;
    for (int i = 0; i < push_count; ++i)
    {
      int pushed = rand();
      existing_elements.emplace(pushed);
      pd.push(pushed);
    }

    int minval = *existing_elements.begin();
    int maxval = minval;

    for (auto const & val : existing_elements)
    {
      minval = (val < minval) ? val : minval;
      maxval = (val > maxval) ? val : maxval;
    }

    BOOST_TEST_REQUIRE(pd.maximum() == maxval);
    BOOST_TEST_REQUIRE(pd.top() == maxval);
    BOOST_TEST_REQUIRE(pd.minimum() == minval);
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_pop_max )
{
  using namespace boost::container;
  for (int push_count = 1; push_count < 26; ++push_count)
  {
    priority_deque<int> pd;
    BOOST_TEST_REQUIRE((pd.begin() == pd.end()));

    std::multiset<int> existing_elements;
    for (int i = 0; i < push_count; ++i)
    {
      int pushed = rand();
      existing_elements.emplace(pushed);
      pd.push(pushed);
    }

    for (int i = 0; i < push_count; ++i)
    {
      int minval = *existing_elements.begin();
      int maxval = minval;

      for (auto const & val : existing_elements)
      {
        minval = (val < minval) ? val : minval;
        maxval = (val > maxval) ? val : maxval;
      }

      BOOST_TEST_REQUIRE(pd.maximum() == maxval);
      BOOST_TEST_REQUIRE(pd.minimum() == minval);
      auto maxit = existing_elements.find(maxval);
      existing_elements.erase(maxit);
      pd.pop_maximum();
      BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
    }
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_pop_min )
{
  using namespace boost::container;
  for (int push_count = 1; push_count < 26; ++push_count)
  {
    priority_deque<int> pd;
    BOOST_TEST_REQUIRE((pd.begin() == pd.end()));

    std::multiset<int> existing_elements;
    for (int i = 0; i < push_count; ++i)
    {
      int pushed = rand();
      existing_elements.emplace(pushed);
      pd.push(pushed);
    }

    for (int i = 0; i < push_count; ++i)
    {
      int minval = *existing_elements.begin();
      int maxval = minval;

      for (auto const & val : existing_elements)
      {
        minval = (val < minval) ? val : minval;
        maxval = (val > maxval) ? val : maxval;
      }

      BOOST_TEST_REQUIRE(pd.maximum() == maxval);
      BOOST_TEST_REQUIRE(pd.minimum() == minval);
      auto minit = existing_elements.find(minval);
      existing_elements.erase(minit);
      pd.pop_minimum();
      BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
    }
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_swap )
{
  using namespace boost::container;
  for (int push_count = 1; push_count < 26; push_count += 8)
  {
    priority_deque<int> pd [2];
    std::multiset<int> st [2];
    for (int i = 0; i < push_count; ++i)
    {
      int pushed = rand();
      int push_loc = rand() & 1;
      st[push_loc].emplace(pushed);
      pd[push_loc].push(pushed);
    }

    BOOST_TEST_REQUIRE(have_same_elements(pd[0], st[0]));
    BOOST_TEST_REQUIRE(have_same_elements(pd[1], st[1]));
    swap(pd[0], pd[1]);
    BOOST_TEST_REQUIRE(have_same_elements(pd[1], st[0]));
    BOOST_TEST_REQUIRE(have_same_elements(pd[0], st[1]));
    pd[0].swap(pd[1]);
    BOOST_TEST_REQUIRE(have_same_elements(pd[0], st[0]));
    BOOST_TEST_REQUIRE(have_same_elements(pd[1], st[1]));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_random_update )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 313; ++i)
    existing_elements.emplace(rand());
  priority_deque<int> pd { existing_elements.begin(), existing_elements.end() };

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 1024; ++i)
  {
    auto it = pd.begin() + (rand() % pd.size());

    int newval = rand();
    auto set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    existing_elements.emplace(newval);
    pd.update(it, newval);

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_random_update_move )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 313; ++i)
    existing_elements.emplace(rand());
  priority_deque<int> pd { existing_elements.begin(), existing_elements.end() };

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 1024; ++i)
  {
    auto it = pd.begin() + (rand() % pd.size());

    int newval = rand();
    auto set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    existing_elements.emplace(newval);
    pd.update(it, std::move(newval));

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_random_erase )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 313; ++i)
    existing_elements.emplace(rand());
  priority_deque<int> pd { existing_elements.begin(), existing_elements.end() };

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 313; ++i)
  {
    auto it = pd.begin() + (rand() % pd.size());

    auto set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    pd.erase(it);

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

