/*----------------------------------------------------------------------------*\
|   Copyright (C) 2013 Nathaniel McClatchey                                    |
|   Released under the Boost Software License Version 1.0, which may be found  |
| at http://www.boost.org/LICENSE_1_0.txt                                      |
\*----------------------------------------------------------------------------*/

/*! @file interval_heap.hpp
//    interval_heap.hpp provides functions for creating interval heaps by
//  rearranging a range of elements. A valid interval heap has the following
//  properties:
//    Given elements A and B, A is less than B iff compare(A,B), where compare
//    is a binary functor describing a strict weak ordering.
//    Left and right bounds are those with even and odd indices, respectively.
//    A right bound is never less than its corresponding left bound.
//    A left bound is never less than the left bound of its parent.
//    A right bound is never less than the right bounds of its children.
//  @note Exception safety: All functions in interval_heap.hpp provide at
//  least the basic exception-safety guarantee. That is, no element will be lost
//  in the event of an exception. Elements may, however, no longer form an
//  interval heap.
//  @note Exception safety: Pre-C++11: If the comparison and swap don't throw,
//  neither do any of the interval-heap functions.
//  @note Exception safety: C++11: If the comparison, move-assign, and
//  move-construct, do not throw, neither do any of the interval-heap functions.
//  @note Thread safety: No static variables are modified by any operation.
//  @note Thread safety: Simultaneous read-write or write-write operations are
//  unsafe.
*/

#ifndef BOOST_HEAP_INTERVAL_HEAP_HPP_
#define BOOST_HEAP_INTERVAL_HEAP_HPP_

#ifndef __cplusplus
#error interval_heap.hpp requires a C++ compiler.
#endif

//  Grab std::swap and std::move (if available), while avoiding extra baggage.
#if (__cplusplus >= 201103L)
#include <utility>
#else
#include <algorithm>
#endif
//  Grab iterator data, for better use of templates.
#include <iterator>

//  Bulk-loading operation is well-suited to threading.
#ifndef BOOST_HEAP_INTERVAL_HEAP_USE_STD_THREAD
#define BOOST_HEAP_INTERVAL_HEAP_USE_STD_THREAD (__cplusplus >= 201103L)
#endif
#if (BOOST_HEAP_INTERVAL_HEAP_USE_STD_THREAD == true)
#include <thread>
#endif

namespace boost {
namespace heap {
//------------------------------User-Accessible---------------------------------
//! @brief Moves elements in [first,last) to form an interval heap.
template <typename Iterator, typename Compare>
void make_interval_heap (Iterator first, Iterator last, Compare compare);

//! @brief Expands the interval heap to include the element at last-1.
template <typename Iterator, typename Compare>
void push_interval_heap (Iterator first, Iterator last, Compare compare);

//!@{
//! @brief Moves a minimal element to the back of the range for popping.
template <typename Iterator, typename Compare>
void pop_interval_heap_min (Iterator first, Iterator last, Compare compare);
//! @brief Moves a maximal element to the back of the range for popping.
template <typename Iterator, typename Compare>
void pop_interval_heap_max (Iterator first, Iterator last, Compare compare);

//! @brief Moves an element to the back of the range for popping.
template <typename Iterator, typename Compare>
void pop_interval_heap (Iterator first, Iterator last,
          typename std::iterator_traits<Iterator>::difference_type index,
          Compare compare);
//!@}
//! @brief Restores the interval-heap property if one element violates it.
template <typename Iterator, typename Compare>
void update_interval_heap (Iterator first, Iterator last,
           typename std::iterator_traits<Iterator>::difference_type index,
           Compare compare);

//! @brief Sorts an interval heap in ascending order.
template <typename Iterator, typename Compare>
void sort_interval_heap (Iterator first, Iterator last, Compare compare);
//! @brief Finds the largest subrange that qualifies as an interval heap.
template <typename Iterator, typename Compare>
Iterator is_interval_heap_until (Iterator first, Iterator last,Compare compare);
//! @brief Checks whether the range is an interval heap.
template <typename Iterator, typename Compare>
bool is_interval_heap (Iterator first, Iterator last, Compare compare);


//-------------------------------Book-Keeping-----------------------------------
namespace interval_heap_internal {
//! @brief Restores the interval-heap property if one leaf element violates it.
template <typename Iterator, typename Compare>
void sift_leaf (Iterator first, Iterator last,
                typename std::iterator_traits<Iterator>::difference_type index,
                Compare compare);
//! @brief Moves an element up the interval heap.
template <bool left_bound, typename Iterator, typename Offset, typename Compare>
void sift_up (Iterator first, Offset index, Compare compare,Offset limit_child);
//! @brief Moves an element between min/max bounds, and up the interval heap.
template <typename Iterator, typename Offset, typename Compare>
void sift_leaf_max (Iterator first, Iterator last, Offset index,
                    Compare compare, Offset limit_child);
//! @brief Moves an element between min/max bounds, and up the interval heap.
template <typename Iterator, typename Offset, typename Compare>
void sift_leaf_min (Iterator first, Iterator last, Offset index,
                    Compare compare, Offset limit_child);
//! @brief Restores the interval-heap property if one element violates it.
template <bool left_bound, typename Iterator, typename Offset, typename Compare>
void sift_down (Iterator first, Iterator last, Offset index, Compare compare,
                Offset limit_child);
} //  Namespace interval_heap_internal

/*! @details This function restores the interval-heap property violated by the
//  specified element.
//  @param first,last A range of random-access iterators.
//  @param index The offset, from @a first, of the offending element.
//  @param compare A comparison object.
//  @pre @a index is in the range [ 0, @a last - @a first).
//  @pre [ @a first, @a last) is a valid interval heap, if the element at
//  @a first + @a index is ignored.
//  @post [ @a first, @a last) is a valid interval heap.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(log n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee. Strong if they do not throw exceptions.
*/
template <typename Iterator, typename Compare>
void update_interval_heap (Iterator first, Iterator last,
           typename std::iterator_traits<Iterator>::difference_type index,
           Compare compare)
{
  using namespace std;
  using interval_heap_internal::sift_down;
  typedef typename iterator_traits<Iterator>::difference_type Offset;
  if (index & 1)
    sift_down<false, Iterator, Offset, Compare>(first, last, index, compare,2);
  else
    sift_down<true, Iterator, Offset, Compare>(first, last, index, compare, 2);
}

/*! @details This function expands an interval heap from [ @a first, @a last -
//  1) to [ @a first, @a last), maintaining the interval-heap property. This is
//  typically used to add a new element to the interval heap.
//  @param first,last A range of random-access iterators.
//  @param compare A comparison object.
//  @pre [ @a first, @a last - 1) is a valid interval heap.
//  @post [ @a first, @a last) is a valid interval heap.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(log n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee. Strong if they do not throw exceptions.
*/
template <typename Iterator, typename Compare>
void push_interval_heap (Iterator first, Iterator last, Compare compare) {
  using interval_heap_internal::sift_leaf;
  sift_leaf<Iterator, Compare>(first, last, (last - first) - 1, compare);
}

/*! @details This function moves a specified element to the end of the range of
//  iterators. This is typically done so that the element can be efficiently
//  removed (by @a pop_back, for example).
//  @param first,last A range of random-access iterators.
//  @param index The offset, from @a first, of the element to be removed.
//  @param compare A comparison object.
//  @pre [ @a first, @a last) is a valid interval heap.
//  @pre @a index is in the range [ 0, @a last - @a first)
//  @post [ @a first, @a last - 1) is a valid interval heap.
//  @post The element originally at @a first + @a index has been moved to
//  @a last - 1.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(log n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee. Strong if they do not throw exceptions.
*/
template <typename Iterator, typename Compare>
void pop_interval_heap (Iterator first, Iterator last,
          typename std::iterator_traits<Iterator>::difference_type index,
          Compare compare)
{
  using namespace std;
  --last;
  swap(*(first + index), *last);
  try {
    update_interval_heap<Iterator, Compare>(first, last, index, compare);
  } catch (...) {
//  Roll back for strong guarantee.
    swap(*last, *(first + index));
    throw;  //  Re-throw the current exception.
  }
}

/*! @details This function moves a minimal element to the end of the range of
//  iterators. This is typically done so that the element can be efficiently
//  removed (by @a pop_back, for example), but can also be used to sort the
//  range in descending order.
//  @param first,last A range of random-access iterators.
//  @param compare A comparison object.
//  @pre [ @a first, @a last) is a valid interval heap.
//  @post A minimal element from the range has been moved to @a last - 1.
//  @post [ @a first, @a last - 1) is a valid interval heap.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(log n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee. Strong if they do not throw exceptions.
*/
template <typename Iterator, typename Compare>
void pop_interval_heap_min (Iterator first, Iterator last, Compare compare) {
  using namespace std;
  using interval_heap_internal::sift_down;
  typedef typename iterator_traits<Iterator>::difference_type Offset;
  --last;
  swap(*first, *last);
  try {
    sift_down<true, Iterator, Offset, Compare>(first, last, 0, compare, 2);
  } catch (...) {
//  Roll back for strong guarantee.
    swap(*last, *first);
    throw;  //  Re-throw the current exception.
  }
}

/*! @details This function moves a maximal element to the end of the range of
//  iterators. This is typically done so that the element can be efficiently
//  removed (by @a pop_back, for example), but can also be used to sort the
//  range in ascending order.
//  @param first,last A range of random-access iterators.
//  @param compare A comparison object.
//  @pre [ @a first, @a last) is a valid interval heap.
//  @post A maximal element from the range has been moved to @a last - 1.
//  @post [ @a first, @a last - 1) is a valid interval heap.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(log n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee. Strong if they do not throw exceptions.
*/
template <typename Iterator, typename Compare>
void pop_interval_heap_max (Iterator first, Iterator last, Compare compare) {
  using namespace std;
  using interval_heap_internal::sift_down;
  typedef typename iterator_traits<Iterator>::difference_type Offset;
//  Nothing to be done.
  if (last - first <= 2)
    return;
  --last;
  swap(*(first + 1), *last);
  try {
    sift_down<false, Iterator, Offset, Compare>(first, last, 1, compare, 2);
  } catch (...) {
//  Roll back for strong guarantee.
    swap(*last, *(first + 1));
    throw;  //  Re-throw the current exception.
  }
}

/*! @details This function takes an interval heap and sorts its elements in
//  ascending order.
//  @param first,last A range of random-access iterators.
//  @param compare A comparison object.
//  @pre [ @a first, @a last) is a valid interval heap.
//  @post [ @a first, @a last) is sorted in ascending order.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(n log n)
//  @note Exception safety: Basic
*/
template <typename Iterator, typename Compare>
void sort_interval_heap (Iterator first, Iterator last, Compare compare) {
  Iterator cursor = last;
  while (cursor != first) {
//  If this throws, anything I do to try to fix it is also likely to throw.
    pop_interval_heap_max<Iterator, Compare>(first, cursor, compare);
    --cursor;
  }
}

//! @brief Finds the largest subrange that forms a valid interval heap.
//! @note Complexity: O(n)
//! @note Exception safety: No-throw
template <typename Iterator, typename Compare>
Iterator is_interval_heap_until (Iterator first, Iterator last, Compare compare)
{
  using namespace std;
  typedef typename iterator_traits<Iterator>::difference_type Offset;

  Offset index = static_cast<Offset>(0);

  try {
    Offset index_end = last - first;
    while (index < index_end) {
      Iterator cursor = first + index;
//  Check whether it is a valid interval.
      if ((index & 1) && compare(*cursor, *(cursor - 1)))
        return cursor;
      if (index >= 2) {
//  If there exists a parent interval, check for containment.
        Iterator parent = first + ((index / 2 - 1) | 1);
        if (index & 1) {
          if (compare(*parent, *cursor))
            return cursor;
        } else {
          if (compare(*parent, *cursor))
            return cursor;
          if (compare(*cursor, *(parent - 1)))
            return cursor;
        }
      }
      ++index;
    }
  } catch (...) {
    return first + index;
  }
  return last;
}

//! @brief Checks whether the range is a valid interval heap.
//! @note Complexity: O(n)
//! @note Exception safety:No-throw
template <typename Iterator, typename Compare>
bool is_interval_heap (Iterator first, Iterator last, Compare compare) {
  try {
    return (is_interval_heap_until<Iterator,Compare>(first, last, compare)
            == last);
  } catch (...) {
    return false;
  }
}

#if (BOOST_HEAP_INTERVAL_HEAP_USE_STD_THREAD == true)
namespace interval_heap_internal {
//! @brief Minimum number of elements in heap layer before threads branch.
static constexpr int kBranchMin = 1 << 10;
//! @brief Minimum number of elements in heap before threading is considered.
static constexpr int kThreadMin = kBranchMin << 1;
/*    This parallel version of the heap-maker uses divide-and-conquer methods to
//  distribute the task amongst the cores.
*/
//! @brief Internal function for threaded bulk-load.
template <typename Iterator, typename Compare, typename Offset>
void make_block (Iterator first, Iterator last, Compare compare,
                 Offset block_begin, Offset block_end, int threads) {
  using namespace std;
  using interval_heap_internal::sift_down;

  const Offset index_end = last - first;
  const Offset end_parent = index_end / 2 - 1;

  if (block_begin < end_parent) {
/*    Recurse. If reasonable, pass half the task to another thread. Don't split
//  if chunks are tiny. Page updates will cause thrashing.
*/
    const Offset child_begin    = (block_begin + 1) * 2;
    const Offset child_end      = (block_end + 1) * 2;

    if ((threads > 1) && (block_end - block_begin >= kBranchMin)) {
      const Offset child_middle = block_end + block_begin + 2;
//  Branch.
      int split_threads = threads >> 1;
      thread thread_branch(&make_block<Iterator,Compare,Offset>, first, last,
                           compare, child_middle, child_end, split_threads);
      make_block(first, last, compare, child_begin, child_middle,
                 threads - split_threads);
      thread_branch.join();
    } else
      make_block(first, last, compare, child_begin, child_end, threads);
  } else
    if (block_end > index_end)
//  If the final interval is a singleton, it's already OK. Skip it.
      block_end = index_end ^ (index_end & 1);

/*    Make this layer of the interval heap; we assume that all lower layers are
//  already OK.
*/
  for (Offset index = block_end - 2; (index >= block_begin); index -= 2) {
    const Offset coindex = index | 1; //  = index + 1
    const Offset stop = (index <= end_parent) ? (coindex * 2) : index_end;
//  If compare throws, heap property cannot be verified or enforced.
//  If swap throws, heap property is violated and cannot be enforced.
    if (compare(*(first + coindex), *(first + index)))
      swap(*(first + coindex), *(first + index));

    sift_down<false, Iterator, Offset, Compare>(first, last, coindex,
                                                compare, stop);
    sift_down<true , Iterator, Offset, Compare>(first, last, index,
                                                compare, stop);
  }
}
}

/*! @details This function moves elements in a range to from an interval-heap.
//  @param first,last A range of random-access iterators.
//  @param compare A comparison object.
//  @post [ @a first, @a last) is a valid interval heap.
//  @invariant No element is added to or removed from the range.
//  @note Complexity: O(n)
//  @note Exception safety: Basic if move/copy/swap provide the basic
//  guarantee.
//  @remark Threaded.
*/
template <typename Iterator, typename Compare>
void make_interval_heap (Iterator first, Iterator last, Compare compare) {
  typedef typename std::iterator_traits<Iterator>::difference_type Offset;
//  Double-heap property holds vacuously.
  if (last - first < 2)
    return;
  int threads = ((last - first) > interval_heap_internal::kThreadMin) ?
                std::thread::hardware_concurrency() : 1;
  interval_heap_internal::make_block<Iterator, Compare, Offset>(first, last,
                                                                compare, 0, 2,
                                                                threads);
}
#else //  Thread-free version, for compilers that don't support std::thread
template <typename Iterator, typename Compare>
void make_interval_heap (Iterator first, Iterator last, Compare compare) {
  using namespace std;
  using interval_heap_internal::sift_down;
  typedef typename iterator_traits<Iterator>::difference_type Offset;

  const Offset index_end = last - first;
//  Double-heap property holds vacuously.
  if (index_end <= 1)
    return;
//  Prevents overflow when number of elements approaches maximum possible index.
  const Offset end_parent = index_end / 2 - 1;
//  If the final interval is a singleton, it's already OK. Skip it.
  Offset index = index_end ^ (index_end & 1);
  do {
    index -= 2;
    const Offset coindex = index | 1; //  = index + 1
    const Offset stop = (index <= end_parent) ? (coindex * 2) : index_end;
//  If compare throws, heap property cannot be verified or enforced.
//  If swap throws, heap property is violated and cannot be enforced.
    if (compare(*(first + coindex), *(first + index)))
      swap(*(first + coindex), *(first + index));

    sift_down<false, Iterator, Offset, Compare>(first, last, coindex,
                                               compare, stop);
    sift_down<true , Iterator, Offset, Compare>(first, last, index,
                                               compare, stop);

  } while (index >= 2);
}
#endif

namespace interval_heap_internal {
//! @remark Exception safety: Strong if move/swap doesn't throw.
template <bool left_bound, typename Iterator, typename Offset, typename Compare>
void sift_up (Iterator first, Offset origin, Compare compare,Offset limit_child)
{
//  Use the most specialized available functions.
  using namespace std;
  typedef typename iterator_traits<Iterator>::value_type Value;
//  Keeping the information about the origin permits strong exception guarantee.
  Offset index = origin;
#if (__cplusplus >= 201103L)  //  C++11
//  Float element in limbo while sifting it up the heap.
  Value limbo = std::move_if_noexcept(*(first + index));
#endif
//  Provides strong exception-safety guarantee (rollback), unless move throws.
  try {
    while (index >= limit_child) {
      const Offset parent = ((index / 2 - 1) | 1) ^ (left_bound ? 1 : 0);
#if (__cplusplus >= 201103L)  //  C++11
      if (compare((left_bound ? limbo : *(first + parent)),
                  (left_bound ? *(first + parent) : limbo))) {
        *(first + index) = std::move_if_noexcept(*(first + parent));
#else
      if (compare(*(first + (left_bound ? index : parent)),
                  *(first + (left_bound ? parent : index)))) {
        swap(*(first + index), *(first + parent));
#endif
        index = parent;
      } else
        break;
    }
  } catch (...) { //  Provides strong exception-safety guarantee.
//  I need limbo because elements are being moved in the direction of travel.
#if (__cplusplus < 201103L)  //  Not C++11. Need to allocate limbo.
    Value limbo;
    swap(*(first + index), limbo);
#endif
    swap(*(first + origin), limbo);
    while (origin > index) {
      origin = ((origin / 2 - 1) | 1) ^ (left_bound ? 1 : 0);
      swap(*(first + origin), limbo);
    }
    throw;  //  Re-throw the current exception.
  }
#if (__cplusplus >= 201103L)  //  C++11
//  Done sifting. Get the element out of limbo.
  *(first + index) = std::move_if_noexcept(limbo);
#endif
}

//! @remark Exception safety: As strong as sift_up.
//! @pre @a index refers to a leaf node of the heap.
template <typename Iterator, typename Offset, typename Compare>
void sift_leaf_max (Iterator first, Iterator last, Offset index,
                    Compare compare, Offset limit_child)
{
//  Use the most specialized swap function.
  using namespace std;
  const Offset index_end = last - first;


//  Index of corresponding left-bound (min) element
  const Offset co_index = ((index_end - 1) / 2 < index)
                            ? (index ^ 1) : (index * 2);
  if (compare(*(first + index), *(first + co_index))) {
    swap(*(first + index), *(first + co_index));
    try { //  Provides strong exception-safety guarantee, unless move throws.
      sift_up<true, Iterator, Offset, Compare>(first, co_index, compare,
                                               limit_child);
    } catch (...) { //  Rollback for strong guarantee.
      swap(*(first + index), *(first + co_index));
      throw;  //  Re-throw the current exception.
    }
  } else
    sift_up<false, Iterator, Offset, Compare>(first, index, compare,
                                              limit_child);
}

//! @remark Exception safety: As strong as sift_up.
//! @pre @a index refers to a leaf node of the heap.
template <typename Iterator, typename Offset, typename Compare>
void sift_leaf_min (Iterator first, Iterator last, Offset index,
                    Compare compare, Offset limit_child)
{
//  Use the most specialized swap function.
  using namespace std;
  const Offset index_end = last - first;

//  Index of corresponding element (initial assumption)
  Offset co_index = index | 1;
//  Co-index is past the end of the heap. Move to its parent, if possible.
  if (co_index >= index_end) {
//  Only one element.
    if (co_index == 1)
      return;
    co_index = (co_index / 2 - 1) | 1;
  }
  if (compare(*(first + co_index), *(first + index))) {
    swap(*(first + index), *(first + co_index));
    try { //  Provides strong exception-safety guarantee, unless move throws.
      sift_up<false, Iterator, Offset, Compare>(first, co_index, compare,
                                                limit_child);
    } catch (...) { //  Rollback for strong guarantee.
      swap(*(first + index), *(first + co_index));
      throw;  //  Re-throw the current exception.
    }
  } else
    sift_up<true, Iterator, Offset, Compare>(first, index, compare,limit_child);
}

//! @remark Exception safety: As strong as sift_up.
template <bool left_bound, typename Iterator, typename Offset, typename Compare>
void sift_down (Iterator first, Iterator last, Offset origin, Compare compare,
                Offset limit_child)
{
//  Use the most specialized available functions.
  using namespace std;
  typedef typename iterator_traits<Iterator>::value_type Value;

//  By keeping track of where I started, I can roll back all changes.
  Offset index = origin;
#if (__cplusplus >= 201103L)  //  C++11
  Value limbo = std::move_if_noexcept(*(first + index));
#endif

  const Offset index_end = last - first;
//  One past the last element with two children.
  const Offset end_parent = index_end / 2 -
                              ((left_bound && ((index_end & 3) == 0)) ? 2 : 1);
  try { //  This try-catch block rolls back after exceptions.
    while (index < end_parent) {
      Offset child = index * 2 + (left_bound ? 2 : 1);
//  If compare throws, heap property cannot be verified or enforced.
      try { //  This try-catch block ensures no element is left in limbo.
        if (compare(*(first + child + (left_bound ? 2 : 0)),
                    *(first + child + (left_bound ? 0 : 2))))
          child += 2;
      } catch (...) {
#if (__cplusplus >= 201103L)  //  C++11
//  Pull the moving element out of limbo, to avoid leaks.
        *(first + index) = std::move_if_noexcept(limbo);
#endif
        throw;  //  Re-throw the current exception.
      }
#if (__cplusplus >= 201103L)  //  C++11
      *(first + index) = std::move_if_noexcept(*(first + child));
#else
      swap(*(first + index), *(first + child));
#endif
      index = child;
    }
//  Special case when index has exactly one child.
    if (index <= end_parent + (left_bound ? 0 : 1)) {
      Offset child = index * 2 + (left_bound ? 2 : 1);
      if (child < index_end) {
//  Need to treat singletons (child + 1) as both upper and lower bounds.
        if (!left_bound && (child + 1 < index_end)) {
//  Calculating this outside the if-statement simplifies exception-handling.
          bool swap_required;
          try {
            swap_required = compare(*(first + child), *(first + (child + 1)));
          } catch (...) {
//  Pull the moving element out of limbo.
#if (__cplusplus >= 201103L)
            *(first + index) = std::move_if_noexcept(limbo);
#endif
            throw;  //  Re-throw the current exception.
          }
          if (swap_required) {
            ++child;
#if (__cplusplus >= 201103L)  //  C++11
            *(first + index) = std::move_if_noexcept(*(first + child));
            *(first + child) = std::move_if_noexcept(limbo);
#else
            swap(*(first + index), *(first + child));
#endif
            index = child;  //  Important for the rollback.
            sift_leaf_min<Iterator, Offset, Compare>(first, last, index,
                                                     compare, limit_child);
            return;
          }
        }
#if (__cplusplus >= 201103L)  //  C++11
        *(first + index) = std::move_if_noexcept(*(first + child));
#else
        swap(*(first + index), *(first + child));
#endif
        index = child;
      }
    }
//  Pull the moving element out of limbo.
#if (__cplusplus >= 201103L)  //  C++11
    *(first + index) = std::move_if_noexcept(limbo);
#endif
    if (left_bound)
      sift_leaf_min<Iterator, Offset, Compare>(first, last, index, compare,
                                               limit_child);
    else
      sift_leaf_max<Iterator, Offset, Compare>(first, last, index, compare,
                                               limit_child);
  } catch (...) {
//  Rolls back comparison exceptions. Move exceptions can't be reliably fixed.
    while (index > origin) {
      const Offset parent = ((index / 2 - 1) | 1) ^ (left_bound ? 1 : 0);
      swap(*(first + parent), *(first + index));
      index = parent;
    }
    throw;  //  Re-throw the current exception.
  }
}

//! @remark Exception safety: As strong as sift_up.
template <typename Iterator, typename Compare>
void sift_leaf (Iterator first, Iterator last,
                typename std::iterator_traits<Iterator>::difference_type index,
                Compare compare)
{
  using namespace std;
  typedef typename iterator_traits<Iterator>::difference_type Offset;
  if (index & 1)
    sift_leaf_max<Iterator, Offset, Compare>(first, last, index, compare, 2);
  else
    sift_leaf_min<Iterator, Offset, Compare>(first, last, index, compare, 2);
}
} //  Namespace interval_heap_internal
} //  Namespace heap
} //  Namespace boost

#endif // BOOST_HEAP_INTERVAL_HEAP_HPP_
