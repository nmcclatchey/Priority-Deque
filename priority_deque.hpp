/*----------------------------------------------------------------------------*\
|   Copyright (C) 2012-2013 Nathaniel McClatchey                               |
|   Released under the Boost Software License Version 1.0, which may be found  |
| at http://www.boost.org/LICENSE_1_0.txt                                      |
\*----------------------------------------------------------------------------*/

/*! @file priority_deque.hpp
//    priority_deque.hpp provides the class priority_deque as a thin wrapper
//  around the functions provided by interval_heap.hpp.
//  @remark Exception-safety: If the means of movement -- move, copy, or swap,
//  depending on exception specifications -- do not throw, the guarantee is as
//  strong as that of the action on the underlying container. (Unless otherwise
//  specified)
//  @note Providing a stronger guarantee is impractical.
*/

#ifndef BOOST_CONTAINER_PRIORITY_DEQUE_HPP_
#define BOOST_CONTAINER_PRIORITY_DEQUE_HPP_

#ifndef __cplusplus
#error priority_deque.hpp requires a C++ compiler.
#endif

//  Default comparison (std::less)
#include <functional>
//  Default container (std::vector)
#include <vector>
//  Grab std::move, if available.
#if (__cplusplus >= 201103L)
#include <utility>
#endif
//  Interval-heap management.
#include "interval_heap.hpp"

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
 *  insertion and access to both ends of a weakly-ordered list of elements.
 *  As a container adaptor, priority_deque is implemented on top of another
 *  container type. By default, this is std::vector, but a different container
 *  may be specified explicitly.
 *  Although the priority deque does permit iteration through its elements,
 *  there is no ordering guaranteed, as different implementations may benefit
 *  from different structures, and all iterators should be discarded after using
 *  any function not labeled const.
 *  @note %priority_deque does not provide a stable ordering. If both A<B and
 *  B<A are false, then the order in which they appear may differ from the order
 *  in which they were added to the priority deque.
 *  @remark %priority_deque replicates the interface of the STL
 *  @a priority_queue class template.
 *  @remark %priority_deque is most useful when removals are interspersed with
 *  insertions. If no further insertions are to be performed after the first
 *  removal, consider using an array and a sorting algorithm instead.
 *  @remark %priority_deque sorts elements as they are added, removed, and
 *  modified by its member functions. If the elements are modified by some means
 *  other than the public member functions, the order must be restoreed before
 *  the priority_deque is used.
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
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (const C& comparison, S&& sequence)
  : sequence_(std::move(sequence)), compare_(comparison)
{
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
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
  try {
    sequence_.insert(sequence_.end(), first, last);
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
  } catch (...) {
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
template <typename InputIterator>
priority_deque<T, S, C>::priority_deque (InputIterator first,InputIterator last,
                                         const C& comparison, S&& sequence)
: sequence_(std::move(sequence)), compare_(comparison) {
  try {
    sequence_.insert(sequence_.end(), first, last);
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
  } catch (...) {
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
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
  try {
    sequence_.push_back(value);
  } catch (...) {
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#if (__cplusplus >= 201103L)
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::push (T&& value) {
  try {
    sequence_.push_back(std::move(value));
  } catch (...) {
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}

template <typename T, typename Sequence, typename Compare>
template<typename... Args>
void priority_deque<T, Sequence, Compare>::emplace (Args&&... args) {
  try {
    sequence_.emplace_back(std::forward<Args>(args)...);
  } catch (...) {
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
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
  const_iterator it = sequence_.begin() + 1;
  return (it == sequence_.end()) ? sequence_.front() : *it;
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
  return sequence_.front();
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
void priority_deque<T, Sequence, Compare>::pop_minimum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no maximal element. Removal impossible.");
  heap::pop_interval_heap_min(sequence_.begin(), sequence_.end(), compare_);
  try {
    sequence_.pop_back();
  } catch (...) {
//  If pop_back has a strong guarantee, this will restore the heap.
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
}
/** @pre  Priority deque contains one or more elements.
//  @post A minimal element has been removed from the priority deque.
//  @post Indices and references are invalidated.
//  @see  minimum, pop_maximum
//
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_maximum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no minimal element. Removal undefined.");
  heap::pop_interval_heap_max(sequence_.begin(), sequence_.end(), compare_);
  try {
    sequence_.pop_back();
  } catch (...) {
//  If pop_back has a strong guarantee, this will restore the heap.
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
}

//--------------------------Whole-Deque Operations-----------------------------|
//-----------------------------------Merge-------------------------------------|
/** @param first, last Input iterators bounding the range [ @a first, @a last)
//  @post Priority deque contains its original elements, and copies of those in
//  the range.
//  @post Indices and references are invalidated.
//
//  @remark Complexity: O(n)
//  @remark Exception safety: Basic.
*/
template <typename T, typename S, typename C>
template <typename InputIterator>
void priority_deque<T, S, C>::merge (InputIterator first, InputIterator last) {
  try {
    sequence_.insert(sequence_.end(), first, last);
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
  } catch (...) { //  Try to fix the problem.
    heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
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
//  @remark Exception safety: Basic. Exceptions won't lose elements, but may
//  corrupt the heap.
*/
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::set (const_iterator it, const T& replacement) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't set element.");
//  Providing the strong guarantee would require saving a copy.
  *(sequence_.begin() + index) = replacement;
  heap::sift_interval_heap(sequence_.begin(),sequence_.end(),index,compare_);
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::set (const_iterator it, T&& replacement) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't set element.");
//  Providing the strong guarantee would require saving a copy.
  *(sequence_.begin() + index) = std::move(replacement);
  heap::sift_interval_heap(sequence_.begin(),sequence_.end(),index,compare_);
}
#endif

/** @param  index  An index in the range [0, size)
//  @pre  Priority deque contains one or more elements.
//  @pre  @a index must be in the range [0, size)
//  @post The deque no longer contains the element previously at @a index.
//  @post Indices and references are invalidated.
//  @see set
//  @remark Complexity: O(log n)
*/
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::erase (const_iterator it) {
  const difference_type index = it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't erase element.");
  heap::pop_interval_heap(sequence_.begin(), sequence_.end(),index,compare_);
  try {
    sequence_.pop_back();
  } catch (...) {
//  If pop_back has the strong guarantee, this will re-make the interval heap.
    heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
    throw;
  }
}

} //  Namespace boost::container
} //  Namespace boost

#endif
