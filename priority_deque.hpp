/*----------------------------------------------------------------------------*\
|   Copyright (C) 2012 Nathaniel McClatchey                                    |
|   Released under the Boost Software License Version 1.0, which may be found  |
| at http://www.boost.org/LICENSE_1_0.txt                                      |
\*----------------------------------------------------------------------------*/

#ifndef BOOST_CONTAINER_PRIORITY_DEQUE_HPP_
#define BOOST_CONTAINER_PRIORITY_DEQUE_HPP_

#ifndef __cplusplus
#error priority_deque.hpp requires a C++ compiler.
#endif

//  Default comparison (std::less)
#include <functional>
//  Default container (std::vector)
#include <vector>

//  Choose the best available version of the assert macro.
#ifdef BOOST_ASSERT_MSG
#define BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(x,m) BOOST_ASSERT_MSG(x,m)
#elif defined assert
#define BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(x,m) assert(x)
#else
#define BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(x,m)
#endif

namespace boost {
namespace container {
//! @brief Efficient double-ended priority queue.
template <typename Type, typename Sequence =std::vector<Type>,
          typename Compare =::std::less<typename Sequence::value_type> >
class priority_deque;

//! @brief Swaps the elements of two priority deques.
template <typename Type, typename Sequence, typename Compare>
void swap (priority_deque<Type, Sequence, Compare>& deque1,
           priority_deque<Type, Sequence, Compare>& deque2);

//----------------------------Priority Deque Class-----------------------------|
/*! @brief Efficient double-ended priority queue.
 *  @author Nathaniel McClatchey
 *  @copyright Boost Software License Version 1.0
 *  @param Type Type of elements in the priority deque.
 *  @param Sequence Underlying sequence container. Must provide random-access
 *  iterators, %front(), %push_back(Type const &), and %pop_back().
 *  Defaults to std::vector<Type>.
 *  @param Compare Comparison class. %Compare(A, B) should return true if %A
 *  should be placed earlier than %B in a strict weak ordering.
 *  Defaults to std::less<Type>, which encapsulates operator<.
 *  @details Priority deques are adaptors, designed to provide efficient
 *  insertion and access both to elements of highest priority and to elements of
 *  lowest priority.
 *  As a container adaptor, priority_deque is implemented on top of another
 *  container type. By default, this is std::vector, but a different container
 *  may be specified explicitly.
 *  Although the priority deque does permit iteration through its elements,
 *  there is no ordering guaranteed, as different implementations may benefit
 *  from different structures, and any non-const function invalidates all
 *  iterators.
 *  @note %priority_deque does not provide a stable ordering. If both A<B and
 *  B<A are false, then the order in which they are accessed may differ from the
 *  order in which they were added to the priority deque.
 *  @remark %priority_deque replicates the interface of the STL
 *  @a priority_queue class template.
 *  @remark %priority_deque is most useful when removals are interspersed with
 *  insertions. If no further insertions are to be performed after the first
 *  removal, consider using an array and sorting algorithm instead.
 *  @remark %priority_deque sorts elements as they are added, removed, and
 *  modified by its member functions. If the elements are modified by some means
 *  other than the public member functions, the order is no longer guaranteed.
 *  @remark Tests with GCC 4.8 indicated that choosing std::vector as the
 *  underlying container gives better performance than std::deque.
 *  @see priority_queue
 */
template <typename Type, typename Sequence, typename Compare>
class priority_deque {
//----------------------------------Public-------------------------------------|
 public:
//---------------------------------Typedefs------------------------------------|
//! @details Underlying container type.
  typedef          Sequence                           container_type;
  typedef typename container_type::value_type         value_type;
//! @details STL Container specifies that this is an unsigned integral type.
  typedef typename container_type::size_type          size_type;
//! @details May be used to examine, but not modify, an element in the deque.
  typedef typename container_type::const_reference    const_reference;
  typedef typename container_type::reference          reference;
  typedef typename container_type::pointer            pointer;
//! @details May be used to examine, but not modify, elements in the deque.
  typedef typename container_type::const_iterator     const_iterator;
  typedef typename container_type::iterator           iterator;
//! @details STL Container specifies that this is a signed integral type.
  typedef typename container_type::difference_type    difference_type;
//-------------------------------Constructors----------------------------------|
//! @brief Constructs a new priority deque.
#if (__cplusplus >= 201103L)
  explicit priority_deque             (const Compare& =Compare(),
                                       Sequence&& =Sequence());
//! @overload
  priority_deque                      (const Compare&, const Sequence&);
#else
  explicit priority_deque             (const Compare& comparison =Compare(),
                                       const Sequence& container =Sequence());
#endif
//! @brief Constructs a new priority deque from a sequence of elements.
#if (__cplusplus >= 201103L)
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       const Compare& comparison =Compare(),
                                       Sequence&& container =Sequence());
//! @overload
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       const Compare&, const Sequence&);
#else
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       const Compare& =Compare(),
                                       const Sequence& =Sequence());
#endif
//-----------------------------Restricted Access-------------------------------|
//! @brief Copies an element into the priority deque.
  void                    push        (const Type& x);
#if (__cplusplus >= 201103L)
//!@overload
  void                    push        (Type&& x);
//! @brief Emplaces an element into the priority deque.
  template<typename... Args>
  void                    emplace     (Args&&... args);
#endif

//!@{
//! @brief Accesses a maximal element in the deque.
  const_reference         maximum     (void) const;
//! @brief Accesses a minimal element in the deque.
  const_reference         minimum     (void) const;
//! @details Identical to std::priority_queue top(). @see @a maximum
  inline const_reference  top         (void) const  { return maximum(); };
//!@}
//!@{
//! @brief Removes a maximal element from the deque.
  void                    pop_maximum (void);
//! @brief Removes a minimal element from the deque.
  void                    pop_minimum (void);
//! @details Identical to std::priority_queue pop(). @see @a pop_maximum
  inline void             pop         (void)        { pop_maximum(); };
//!@}

//--------------------------------Deque Size-----------------------------------|
//!@{
//! @brief Returns true if the priority deque is empty, false if it is not.
  inline bool             empty       (void) const  {return sequence_.empty();};
//! @brief Returns the number of elements in the priority deque.
  inline size_type        size        (void) const  {return sequence_.size(); };
//! @brief Returns the maximum number of elements that can be contained.
  inline size_type        max_size    (void) const  {
    return sequence_.max_size();
  };
//!@}

//--------------------------Whole-Deque Operations-----------------------------|
//! @brief Removes all elements from the priority deque.
  inline void             clear       (void)        { sequence_.clear();      };
//! @brief Moves the elements from this deque into another, and vice-versa.
  void                    swap        (priority_deque<Type, Sequence,Compare>&);
//!@{
//! @brief Merges a sequence of elements into the priority deque.
  template <typename InputIterator>
  void                    merge       (InputIterator first, InputIterator last);
//! @brief Merges a container's elements into the priority deque.
  template <typename SourceContainer>
  inline void             merge       (const SourceContainer& source) {
    merge(source.begin(), source.end());
  }
//!@}

//-------------------------------Random Access---------------------------------|
//!@{
//! @brief Returns a const iterator at the beginning of the sequence.
  inline const_iterator   begin       (void) const  {return sequence_.begin();};
//! @brief Returns a const iterator past the end of the sequence.
  inline const_iterator   end         (void) const  { return sequence_.end(); };
//! @brief Modifies a specified element of the deque.
  void                    set         (const_iterator it, const Type& x);
#if (__cplusplus >= 201103L)
//!@overload
  void                    set         (const_iterator it, Type&& x);
#endif
//! @brief Removes a specified element from the deque.
  void                    erase       (const_iterator it);
//!@}

//--------------------------------Protected------------------------------------|
 protected:
//!@{
//!   Direct access to the underlying container. Useful when large portions of
//! the deque must be modified, such as in some methods of stabilization. These
//! functions are protected to prevent unintentional access; use with caution.
//! @brief Returns a const iterator at the beginning of the sequence.
  inline iterator         begin_mutable (void)    { return sequence_.begin(); };
//! @brief Returns a const iterator past the end of the sequence.
  inline iterator         end_mutable   (void)    { return sequence_.end(); };
//!@}
//! @brief Restores validity after the underlying container has been modified.
  void                    make_valid    (void);
//---------------------------------Private-------------------------------------|
 private:
//  Encapsulated functions, for easy maintenance.
//!@{
//! Guaranteed to be mutable, even if @a iterator and @a reference are not.
  typedef typename Sequence::iterator   mutable_iterator;
  typedef typename Sequence::reference  mutable_reference;

//! @brief Compares two elements of the heap, based on whether the comparison is
//! intended for the max (left) heap or min (right) heap.
  template<bool left_heap>
  inline bool compare (reference a, reference b) {
    return left_heap ? compare_(a, b) : compare_(b, a);
  }
//! @brief Choose move semantics in C++11. Encapsulated for easy maintenance.
#if (__cplusplus >= 201103L)
  template<typename T>
  inline static typename std::remove_reference<T>::type&&  safe_move  (T&& t) {
    return std::move(t);
  }
#else
  template<typename T>
  inline static T& safe_move (T& t) { return t; }
#endif
//!@}

//!@{
//! @note limit_child is child of uppermost valid element. 2 if unrestricted.
//! @brief Send a leaf element up the heap.
  template<bool left_heap>
  void  replace_bubble(mutable_iterator first, difference_type index,
                       mutable_reference new_element,
                       difference_type limit_child = 2);
//! @brief Replace any leaf element in the deque.
  template<bool left_heap>
  void  replace_leaf  (mutable_iterator first, difference_type index_end,
                       difference_type index, mutable_reference new_element,
                       difference_type limit_child = 2);
//! @brief Replace any element in the deque.
  template<bool left_heap>
  void  replace       (mutable_iterator first, mutable_iterator last,
                       difference_type index, mutable_reference new_element,
                       difference_type limit_child = 2);
//! @brief Restores the heap property when only the last element is invalid.
  void  make_back_valid   (void);
//!@}

  Sequence sequence_;
  Compare compare_;
};

//-------------------------------Constructors----------------------------------|
//----------------------------Default Constructor------------------------------|
/** @param comparison Comparison class.
//  @param sequence Container class.
//  @post Deque contains copies of all elements from @a sequence.
//
//  @remark Complexity: O(n)
*/
template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (const C& comparison, const S& sequence)
  : sequence_(sequence), compare_(comparison)
{
  make_valid();
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (const C& comparison, S&& sequence)
  : sequence_(std::move(sequence)), compare_(comparison)
{
  make_valid();
}
#endif

//---------------------------Create from Iterators-----------------------------|
/** @param first,last Range of elements.
//  @param comparison Instance of comparison class.
//  @param sequence Instance of container class.
//  @post Deque contains copies of all elements in @a sequence (if specified)
//  and in the range [ @a first, @a last).
//
//  @remark Complexity: O(n)
*/
template <typename T, typename S, typename C>
template <typename InputIterator>
priority_deque<T, S, C>::priority_deque (InputIterator first,InputIterator last,
                                         const C& comparison, const S& sequence)
: sequence_(sequence), compare_(comparison) {
  sequence_.insert(sequence_.end(), first, last);
  make_valid();
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
template <typename InputIterator>
priority_deque<T, S, C>::priority_deque (InputIterator first,InputIterator last,
                                         const C& comparison, S&& sequence)
: sequence_(std::move(sequence)), compare_(comparison) {
  sequence_.insert(sequence_.end(), first, last);
  make_valid();
}
#endif

//-----------------------------Restricted Access-------------------------------|
//------------------------------Insert / Emplace-------------------------------|
/** @param  x Element to add the the priority deque.
//  @post Priority deque contains a copy of @a x.
//  @post Indices and references are invalidated.
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::push (const T& value) {
  sequence_.push_back(value);
  make_back_valid();
}
#if (__cplusplus >= 201103L)
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::push (T&& value) {
  sequence_.push_back(std::move(value));
  make_back_valid();
}

template <typename T, typename Sequence, typename Compare>
template<typename... Args>
void priority_deque<T, Sequence, Compare>::emplace (Args&&... args) {
  sequence_.emplace_back(std::forward<Args>(args)...);
  make_back_valid();
}
#endif

//---------------------------Observe Maximum/Minimum---------------------------|
/** @return Const reference to a maximal element in the priority deque.
//  @pre  Priority deque contains one or more elements.
//  @see  minimum, pop_maximum
//
//  @remark Complexity: O(1)
*/
template <typename T, typename Sequence, typename Compare>
inline typename priority_deque<T, Sequence, Compare>::const_reference
  priority_deque<T, Sequence, Compare>::maximum  (void) const
{
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no maximal element. Reference undefined.");
  return sequence_.front();
}
/** @return Const reference to a minimal element in the priority deque.
//  @pre  Priority deque contains one or more elements.
//  @see  maximum, pop_minimum
//
//  @remark Complexity: O(1)
*/
template <typename T, typename Sequence, typename Compare>
typename priority_deque<T, Sequence, Compare>::const_reference
  priority_deque<T, Sequence, Compare>::minimum  (void) const
{
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no minimal element. Reference undefined.");
  const_iterator it = sequence_.begin() + 1;
  return (it == sequence_.end()) ? sequence_.front() : *it;
}
//---------------------------Remove Maximum/Minimum----------------------------|
/** @pre  Priority deque contains one or more elements.
//  @post A maximal element has been removed from the priority deque.
//  @post Indices and references are invalidated.
//  @see  maximum, pop, pop_minimum
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_maximum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no maximal element. Removal impossible.");
  mutable_iterator sequence_back = sequence_.end() - 1;
  replace<true>(sequence_.begin(), sequence_back, 0, *sequence_back);
  sequence_.pop_back();
}
/** @pre  Priority deque contains one or more elements.
//  @post A minimal element has been removed from the priority deque.
//  @post Indices and references are invalidated.
//  @see  minimum, pop_maximum
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_minimum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no minimal element. Removal undefined.");
  mutable_iterator sequence_back = sequence_.end() - 1;
  if (sequence_back - sequence_.begin() > 1) //  size() > 2
    replace<false>(sequence_.begin(), sequence_back, 1, *sequence_back);
  sequence_.pop_back();
}

//--------------------------Whole-Deque Operations-----------------------------|
//-----------------------------------Merge-------------------------------------|
/** @param first, last Input iterators bounding the range [ @a first, @a last)
//  @post Priority deque contains its original elements, and copies of those in
//  the range.
//  @post Indices and references are invalidated.
//
//  @remark Complexity: O(n)
*/
template <typename T, typename S, typename C>
template <typename InputIterator>
void priority_deque<T, S, C>::merge (InputIterator first, InputIterator last) {
  sequence_.insert(sequence_.end(), first, last);
  make_valid();
}

//----------------------------Swap Specialization------------------------------|
/** @param other Priority deque with which to swap.
//  @post Deque contains the elements from @a source, and @a source contains the
//  elements from this deque.
//  @post Indices and references are invalidated.
//  @note Sequence containers are required to have swap functions.
//  @remark Complexity: O(1)
*/
template <typename T, typename S, typename C>
inline void priority_deque<T, S, C>::swap (priority_deque<T, S, C>& other) {
  sequence_.swap(other.sequence_);
}

/** @relates priority_deque
//  @param deque1,deque2 Priority deques.
//  @post @a deque1 contains the elements originally in @a deque2, and @a deque2
//  contains the elements originally in @a deque1
//
//  @remark Complexity: O(1)
*/
template <typename T, typename S, typename C>
inline void swap (priority_deque<T, S, C>& deque1,
                  priority_deque<T, S, C>& deque2)
{
  deque1.swap(deque2);
}

//---------------------------Random-Access Mutators----------------------------|
/** @param  index An index in the range [0, size)
//  @param  x The new value.
//  @pre  Priority deque contains one or more elements.
//  @pre  @a index must be in the range [0, size)
//  @post The element at @a index is set to @a x.
//  @post Indices and references are invalidated.
//  @see erase
//
//  Elements within the deque may be unordered.
//  @remark Complexity: O(log n)
*/
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::set (const_iterator it, const T& replacement) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't set element.");
//  Need to clone it, in case it's already in the deque.
  value_type swapspace = replacement;
  if (index & 1)
    replace<false>(sequence_.begin(), sequence_.end(), index, swapspace);
  else
    replace<true>(sequence_.begin(), sequence_.end(), index, swapspace);
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::set (const_iterator it, T&& replacement) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't set element.");
  value_type swapspace = std::move(replacement);
  if (index & 1)
    replace<false>(sequence_.begin(), sequence_.end(), index, swapspace);
  else
    replace<true>(sequence_.begin(), sequence_.end(), index, swapspace);
}
#endif

/** @param  index  An index in the range [0, size)
//  @pre  Priority deque contains one or more elements.
//  @pre  @a index must be in the range [0, size)
//  @post The deque no longer contains the element previously at @a index.
//  @post Indices and references are invalidated.
//  @see set
//
//  Elements within the deque may be unordered.
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::erase (const_iterator it) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't erase element.");
  mutable_iterator sequence_back = sequence_.end() - 1;
  if (index & 1)
    replace<false>(sequence_.begin(), sequence_back, index, *sequence_back);
  else
    replace<true> (sequence_.begin(), sequence_back, index, *sequence_back);
  sequence_.pop_back();
}

//-----------------------------Private Functions-------------------------------|
/** @pre  Interval heap property holds for all elements except the last one.
//  @post Interval heap property holds for all elements.
//  @post Indices and references are invalidated.
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::make_back_valid () {
  value_type swapspace = safe_move(sequence_.back());
  const difference_type index_end = sequence_.end() - sequence_.begin();
  if (index_end & 1)
    replace_leaf<true> (sequence_.begin(), index_end, index_end - 1, swapspace);
  else
    replace_leaf<false>(sequence_.begin(), index_end, index_end - 1, swapspace);
}

/** @details When called, rearranges the elements of the priority deque such
//  that they form a valid priority deque.
//  In the current implementation, creates an interval heap.
//  @post Interval heap property holds for all elements.
//  @post Indices and references are invalidated.
//
//  @remark Complexity: O(n)
//  @todo Investigate better memory use. Can probably reduce number of active
//  pages.
//  @todo Investigate threading. Can probably deal with any non-corresponding
//  same-level elements in different threads.
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::make_valid (void) {
  mutable_iterator first = sequence_.begin();
  mutable_iterator last = sequence_.end();
  const difference_type end_index = last - first;
//  Double-heap property holds vacuously.
  if (end_index <= 1)
    return;
//  Prevent overflow when number of elements approaches maximum possible index.
  const difference_type end_parent = end_index / 2 - 1;
//  It's possible for the final interval to be a singleton. If so, skip it.
  difference_type index = end_index ^ (end_index & 1);
  do {
    index -= 2;
    difference_type stop = (index <= end_parent) ? (index * 2 + 2) : end_index;
    if (compare<true>(*(first + index), *(first + index + 1))) {
      value_type swap_space = safe_move(*(first + index));
      *(first + index) = safe_move(*(first + index + 1));
      *(first + index + 1) = safe_move(swap_space);
    }
    value_type swap_min = safe_move(*(first + index + 1));
    replace<false>(first, last, index + 1, swap_min, stop + 1);
    value_type swap_max = safe_move(*(first + index));
    replace<true>(first, last, index, swap_max, stop);
  } while (index > 0);
}

/** @pre Interval heap property holds for all elements with indices greater than
//  or equal to the index of the parent of @a limit_child, except possibly at @a
//  index.
//  @pre  @a new_element is not in the heap.
//  @pre  @a index is the index of a leaf element.
//  @post Interval heap contains @a new_element.
//  @post Interval heap property holds for all elements with indices greater
//  than or equal to the index of the parent of @a limit_child.
//  @post Indices and references are invalidated.
//  @param first Iterator at the beginning of the heap.
//  @param index The element to replace, as an offset from first.
//  @param replacement_element Reference to replacement element. @a element is
//  moved if the compiler supports C++11 and moving is possible, copied if not.
//  @param limit_child Restricts the function to not modify anything above the
//  parent of @a limit_child.
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
template <bool left_heap>
void priority_deque<T, Sequence, Compare>::replace_bubble (
        mutable_iterator first, difference_type index,
        mutable_reference replacement_element, difference_type limit_child)
{
  while (index >= limit_child) {
    const difference_type parent = ((index / 2 - 1) | 1) ^ 1 ^ (index & 1);
    if (compare<left_heap>(*(first + parent), replacement_element)) {
      *(first + index) = safe_move(*(first + parent));
      index = parent;
    } else
      break;
  }
//  Finally, place the replacement element in the proper spot.
  *(first + index) = safe_move(replacement_element);
}
/** @pre Interval heap property holds for all elements with indices greater than
//  or equal to the index of the parent of @a limit_child, except possibly at @a
//  index.
//  @pre  @a new_element is not in the heap.
//  @pre  @a index is the index of a leaf element.
//  @post Interval heap contains @a new_element.
//  @post Interval heap property holds for all elements with indices greater
//  than or equal to the index of the parent of @a limit_child.
//  @post Indices and references are invalidated.
//  @param first Iterator at the beginning of the heap.
//  @param index_end Size of the heap.
//  @param index The element to replace, as an offset from first.
//  @param replacement_element Reference to replacement element. @a element is
//  moved if the compiler supports C++11 and moving is possible, copied if not.
//  @param limit_child Restricts the function to not modify anything above the
//  parent of @a limit_child.
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
template <bool left_heap>
void priority_deque<T, Sequence, Compare>::replace_leaf (
        mutable_iterator first, difference_type index_end,
        difference_type index, mutable_reference replacement_element,
        difference_type limit_child)
{
//  Index of corresponding element (initial assumption)
  difference_type co_index = index ^ 1;
//! @note: (co_index >= (last - first) only if left_heap
//  Co-index is past the end of the heap. Move to its parent, if possible.
  if (left_heap) {
    if (co_index >= index_end) {
//  Avoid modifying anything beyond the parent of the limit child.
      if (co_index < limit_child)
        co_index = index;
      else
        co_index = (index / 2 - 1) | 1;
    }
  } else {
//  If the co-element has a child, go to it.
    if (co_index * 2 + 2 < index_end)
      co_index = co_index * 2 + 2;
  }
//  No overflows from here on, and underflows do not matter.
  if (compare<left_heap>(replacement_element, *(first + co_index))) {
//  Element is in wrong heap. Switch
    *(first + index) = safe_move(*(first + co_index));
    replace_bubble<!left_heap>(first,co_index,replacement_element, limit_child);
  } else {
    replace_bubble<left_heap>(first, index, replacement_element, limit_child);
  }
}

/** @pre Interval heap is not empty.
//  @pre Interval heap property holds for all elements with indices greater than
//  or equal to the index of the parent of @a limit_child, except possibly at @a
//  index.
//  @pre  @a new_element is not in the heap.
//  @post Interval heap contains @a new_element.
//  @post Interval heap property holds for all elements with indices greater than
//  or equal to the index of the parent of @a limit_child.
//  @post Indices and references are invalidated.
//  @param first, last Iterators defining the range [first, last).
//  @param index The element to replace, as an offset from first.
//  @param replacement_element Reference to replacement element. @a element is
//  moved if the compiler supports C++11 and moving is possible, copied if not.
//  @param limit_child Restricts the function to not modify anything above the
//  parent of @a limit_child.
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
template <bool left_heap>
void priority_deque<T, Sequence, Compare>::replace (
        mutable_iterator first, mutable_iterator last,
        difference_type index, mutable_reference replacement_element,
        difference_type limit_child)
{
  const difference_type index_end = last - first;
//  One past the last element with two children.
  const difference_type end_parent = index_end / 2 -
                                ((left_heap && ((index_end & 3) == 0)) ? 2 : 1);
  while (index < end_parent) {
    difference_type child = index * 2 + (left_heap ? 2 : 1);
    if (compare<left_heap>(*(first + child), *(first + child + 2)))
      child += 2;
    *(first + index) = safe_move(*(first + child));
    index = child;
  }
//  Special case when index has exactly one child.
  if (index < end_parent + 2) {
    difference_type child = index * 2 + (left_heap ? 2 : 1);
    if (child < index_end) {
      *(first + index) = safe_move(*(first + child));
      index = child;
    }
  }
  replace_leaf<left_heap>(first, index_end, index, replacement_element,
                          limit_child);
}

} //  Namespace boost::container
} //  Namespace boost

#endif
