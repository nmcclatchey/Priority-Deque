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

struct ThrowingElement
{
  static long throw_on_move_assign;
  static long throw_on_copy_assign;
  static long throw_on_move_construct;
  static long throw_on_copy_construct;
  static long throw_on_compare;

  int value;
  bool valid;

  ThrowingElement (void)
    : value(0), valid(false)
  {
  }

  ThrowingElement (int n)
    : value(n), valid(true)
  {
  }

  ThrowingElement (ThrowingElement const & rhs)
    : value(rhs.value), valid(rhs.valid)
  {
    if (--throw_on_copy_construct == 0)
      throw std::runtime_error("Copy constructor exception.");
  }

  ThrowingElement & operator= (ThrowingElement const & rhs)
  {
    if (--throw_on_copy_assign == 0)
      throw std::runtime_error("Copy assignment exception.");
    value = rhs.value;
    valid = rhs.valid;
    return *this;
  }

#if __cplusplus >= 201103L
  ThrowingElement (ThrowingElement && rhs)
    : value(rhs.value), valid(rhs.valid)
  {
    if (--throw_on_move_construct == 0)
      throw std::runtime_error("Move constructor exception.");
    rhs.valid = false;
  }

  ThrowingElement & operator= (ThrowingElement && rhs)
  {
    if (--throw_on_move_assign == 0)
      throw std::runtime_error("Move assignment exception.");
    value = rhs.value;
    valid = rhs.valid;
    rhs.valid = false;
    return *this;
  }
#endif

  ~ThrowingElement (void)
  {
  }

  bool operator< (ThrowingElement const & rhs) const
  {
    if (--throw_on_compare == 0)
      throw std::runtime_error("Comparison exception.");
    return value < rhs.value;
  }
};

long ThrowingElement::throw_on_move_assign = -1;
long ThrowingElement::throw_on_copy_assign = -1;
long ThrowingElement::throw_on_move_construct = -1;
long ThrowingElement::throw_on_copy_construct = -1;
long ThrowingElement::throw_on_compare = -1;

template<class T>
struct ThrowingContainer
{
  static long throw_on_push;
  static long throw_on_pop;

  std::vector<T> values;

  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::value_type value_type;
  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::size_type size_type;
  typedef typename std::vector<T>::const_reference const_reference;
  typedef typename std::vector<T>::pointer pointer;
  typedef typename std::vector<T>::difference_type difference_type;

  ThrowingContainer (void)
    : values()
  {
  }

  iterator begin (void)
  {
    return values.begin();
  }

  iterator end (void)
  {
    return values.end();
  }

  const_iterator begin (void) const
  {
    return values.begin();
  }

  const_iterator end (void) const
  {
    return values.end();
  }

  void push_back (T const & val)
  {
    if (--throw_on_push == 0)
      throw std::runtime_error("Push exception.");
    values.push_back(val);
  }

#if __cplusplus >= 201103L
  template<class ... Args>
  void emplace_back (Args&&... args)
  {
    if (--throw_on_push == 0)
      throw std::runtime_error("Emplace exception.");
    values.emplace_back(std::forward<Args>(args)...);
  }
#endif

  void pop_back (void)
  {
    if (--throw_on_pop == 0)
      throw std::runtime_error("Pop exception.");
    values.pop_back();
  }

  bool empty (void) const
  {
    return values.empty();
  }

  size_type size (void) const
  {
    return values.size();
  }
};

template<class T>
long ThrowingContainer<T>::throw_on_push = -1;

template<class T>
long ThrowingContainer<T>::throw_on_pop = -1;
}

BOOST_AUTO_TEST_CASE( priority_push_pop_element_exceptions )
{
  using namespace boost::container;
  priority_deque<ThrowingElement> pd;
  BOOST_TEST_REQUIRE(pd.empty());
  for (int i = 0; i < 91; ++i)
  {
    BOOST_TEST_REQUIRE(pd.size() == i);
    pd.push(ThrowingElement(i));
    BOOST_TEST_REQUIRE(!pd.empty());
  }

  priority_deque<ThrowingElement> copied_pd = pd;

  ThrowingElement::throw_on_compare = 3;
  BOOST_REQUIRE_THROW(pd.push(ThrowingElement(36)), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());
  //for (auto it = pd.begin(); it != pd.end(); ++it)

#if __cplusplus >= 201103L
  ThrowingElement::throw_on_compare = 3;
  BOOST_REQUIRE_THROW(pd.emplace(36), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());
#endif

  ThrowingElement::throw_on_compare = 3;
  ThrowingElement reference_element = ThrowingElement(36);
  BOOST_REQUIRE_THROW(pd.push(reference_element), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());

  ThrowingElement::throw_on_compare = 3;
  BOOST_REQUIRE_THROW(pd.pop_minimum(), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());

  ThrowingElement::throw_on_compare = 3;
  BOOST_REQUIRE_THROW(pd.pop_maximum(), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());
}

BOOST_AUTO_TEST_CASE( priority_push_pop_container_exceptions )
{
  using namespace boost::container;
  priority_deque<int, ThrowingContainer<int> > pd;
  BOOST_TEST_REQUIRE(pd.empty());
  for (int i = 0; i < 91; ++i)
  {
    BOOST_TEST_REQUIRE(pd.size() == i);
    pd.push(i);
    BOOST_TEST_REQUIRE(!pd.empty());
  }

  priority_deque<int, ThrowingContainer<int> > copied_pd = pd;

  ThrowingContainer<int>::throw_on_push = 1;
  BOOST_REQUIRE_THROW(pd.push(36), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());

  ThrowingContainer<int>::throw_on_pop = 1;
  BOOST_REQUIRE_THROW(pd.pop_minimum(), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());

  ThrowingContainer<int>::throw_on_pop = 1;
  BOOST_REQUIRE_THROW(pd.pop_maximum(), std::runtime_error);
  BOOST_TEST_REQUIRE(pd.size() == copied_pd.size());
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

