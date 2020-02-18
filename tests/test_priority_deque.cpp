#include "../priority_deque.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cstdlib>
#include <set>

namespace
{
template<class T, class T2>
bool have_same_elements(T const & values, std::multiset<T2> rf)
{
  typedef typename T::const_iterator itr_t;
  for (itr_t it = values.begin(); it != values.end(); ++it)
  {
    typedef typename std::multiset<T2>::iterator set_itr_t;
    set_itr_t set_it = rf.find(*it);
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
      existing_elements.insert(pushed);
      pd.push(pushed);
    }

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_range_constructor_clear )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 512; ++i)
    existing_elements.insert(rand());
  priority_deque<int> pd ( existing_elements.begin(), existing_elements.end() );

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  pd.clear();
  BOOST_TEST_REQUIRE(pd.empty());
}

#if (__cplusplus >= 201103L)
BOOST_AUTO_TEST_CASE( priority_deque_emplace )
{
  using namespace boost::container;

  priority_deque<int> pd;
  std::multiset<int> existing_elements;
  for (int i = 0; i < 512; ++i)
  {
    int newval = rand();
    existing_elements.insert(newval);
    pd.emplace(newval);
  }

  BOOST_TEST_REQUIRE(have_same_elements(pd, std::move(existing_elements)));
}
#endif

BOOST_AUTO_TEST_CASE( priority_deque_insert )
{
  using namespace boost::container;

  std::multiset<int> existing_elements, new_vals;
  for (int i = 0; i < 517; ++i)
    existing_elements.insert(rand());
  for (int j = 0; j < 139; ++j)
    new_vals.insert(rand());
  priority_deque<int> pd;
  pd.insert(existing_elements.begin(), existing_elements.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  pd.insert(new_vals.begin(), new_vals.end());
  existing_elements.insert(new_vals.begin(), new_vals.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
}

BOOST_AUTO_TEST_CASE( priority_deque_insert_few )
{
  using namespace boost::container;

  std::multiset<int> existing_elements, new_vals;
  for (int i = 0; i < 1; ++i)
    existing_elements.insert(rand());
  for (int j = 0; j < 1; ++j)
    new_vals.insert(rand());
  priority_deque<int> pd;
  pd.insert(existing_elements.begin(), existing_elements.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  pd.insert(new_vals.begin(), new_vals.end());
  existing_elements.insert(new_vals.begin(), new_vals.end());
  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
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
      existing_elements.insert(pushed);
      pd.push(pushed);
    }

    int minval = *existing_elements.begin();
    int maxval = minval;

    for (std::multiset<int>::iterator it = existing_elements.begin(); it != existing_elements.end(); ++it)
    {
      minval = (*it < minval) ? *it : minval;
      maxval = (*it > maxval) ? *it : maxval;
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
      existing_elements.insert(pushed);
      pd.push(pushed);
    }

    for (int i = 0; i < push_count; ++i)
    {
      int minval = *existing_elements.begin();
      int maxval = minval;

      for (std::multiset<int>::iterator it = existing_elements.begin(); it != existing_elements.end(); ++it)
      {
        minval = (*it < minval) ? *it : minval;
        maxval = (*it > maxval) ? *it : maxval;
      }

      BOOST_TEST_REQUIRE(pd.maximum() == maxval);
      BOOST_TEST_REQUIRE(pd.minimum() == minval);
      std::multiset<int>::iterator maxit = existing_elements.find(maxval);
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
      existing_elements.insert(pushed);
      pd.push(pushed);
    }

    for (int i = 0; i < push_count; ++i)
    {
      int minval = *existing_elements.begin();
      int maxval = minval;

      for (std::multiset<int>::iterator it = existing_elements.begin(); it != existing_elements.end(); ++it)
      {
        minval = (*it < minval) ? *it : minval;
        maxval = (*it > maxval) ? *it : maxval;
      }

      BOOST_TEST_REQUIRE(pd.maximum() == maxval);
      BOOST_TEST_REQUIRE(pd.minimum() == minval);
      std::multiset<int>::iterator minit = existing_elements.find(minval);
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
      st[push_loc].insert(pushed);
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
    existing_elements.insert(rand());
  priority_deque<int> pd ( existing_elements.begin(), existing_elements.end() );

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 1024; ++i)
  {
    priority_deque<int>::iterator it = pd.begin() + (rand() % pd.size());

    int newval = rand();
    std::multiset<int>::iterator set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    existing_elements.insert(newval);
    pd.update(it, newval);

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_random_update_move )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 313; ++i)
    existing_elements.insert(rand());
  priority_deque<int> pd ( existing_elements.begin(), existing_elements.end() );

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 1024; ++i)
  {
    priority_deque<int>::iterator it = pd.begin() + (rand() % pd.size());

    int newval = rand();
    std::multiset<int>::iterator set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    existing_elements.insert(newval);
    pd.update(it, newval);

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

BOOST_AUTO_TEST_CASE( priority_deque_random_erase )
{
  using namespace boost::container;

  std::multiset<int> existing_elements;
  for (int i = 0; i < 313; ++i)
    existing_elements.insert(rand());
  priority_deque<int> pd ( existing_elements.begin(), existing_elements.end() );

  BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));

  for (int i = 0; i < 313; ++i)
  {
    priority_deque<int>::iterator it = pd.begin() + (rand() % pd.size());

    std::multiset<int>::iterator set_it = existing_elements.find(*it);
    existing_elements.erase(set_it);
    pd.erase(it);

    BOOST_TEST_REQUIRE(have_same_elements(pd, existing_elements));
  }
}

