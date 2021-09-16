/*----------------------------------------------------------------------------*\
|   Copyright (C) 2012-2013 Nathaniel J. McClatchey                            |
|   Released under the Boost Software License Version 1.0, which may be found  |
| at http://www.boost.org/LICENSE_1_0.txt                                      |
\*----------------------------------------------------------------------------*/

/*! @file priority_deque.hpp
//  @author Nathaniel J. McClatchey, PhD
//
//    priority_deque.hpp provides the class priority_deque as a thin wrapper
//  around the functions provided by interval_heap.hpp.
//  @par  Thread safety:
//    No static variables are modified by any operation.  \n
//    Simultaneous const operations are safe.  \n
//    Using any non-const operation without synchronization causes undefined
//  behavior.
//  @par Exception safety:
//    If the means of movement -- move, copy, or swap, depending on exception
//  specifications -- does not throw, the guarantee is as strong as that of the
//  action on the underlying container. (Unless otherwise specified)
*/

#ifndef BOOST_CONTAINER_PRIORITY_DEQUE_HPP_
#define BOOST_CONTAINER_PRIORITY_DEQUE_HPP_

#ifndef __cplusplus
#error priority_deque.hpp requires a C++ compiler.
#endif

//  Grab std::swap and std::move (if available), while avoiding extra baggage.
#if (__cplusplus >= 201103L)
#include <utility>
#else
#include <algorithm>
#endif
//  Grab std::difference, for exception-safety in merge operations.
#include <iterator>
//  Default comparison (std::less)
#include <functional>
//  Default container (std::vector)
#include <vector>

//  Manage Internal heap structure.
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

/** @brief Swaps the elements of two priority deques.
// @relates priority_deque
//  @param deque1,deque2 Priority deques.
//  @post @a deque1 contains the elements originally in @a deque2, and @a deque2
//  contains the elements originally in @a deque1
//  @post All iterators and references are invalidated.
//
//  @par  Complexity:
//    O(1) - Does not depend on the size of the deque.
//  @par Exception safety:
//    No-throw.
*/
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
 *  @note Complexity may depend on the underlying container.
 *  @note Exception safety may depend on the underlying container and value
 *  type. Ensure that Type may be safely moved to benefit from strong exception
 *  safety guarantees.
 *  @remark %priority_deque replicates the interface of the STL
 *  @a priority_queue class template.
 *  @remark %priority_deque is most useful when removals are interspersed with
 *  insertions. If no further insertions are to be performed after the first
 *  removal, consider using an array and a sorting algorithm instead.
 *  @remark %priority_deque structures elements as they are added, removed, and
 *  modified by its member functions. If the elements are modified by some means
 *  other than the public member functions, the order must be restored before
 *  the priority_deque is used.
 *  @see priority_queue
 */
template <typename Type, typename Sequence, typename Compare>
class priority_deque {
//----------------------------------Public-------------------------------------|
 public:
//---------------------------------Typedefs------------------------------------|
//! @details Underlying container type.
  typedef Sequence                                    container_type;
  typedef typename container_type::value_type         value_type;
  typedef Compare                                     value_compare;
//! @details STL Container specifies that this is an unsigned integral type.
  typedef typename container_type::size_type          size_type;
//! @details May be used to examine, but not modify, an element in the deque.
  typedef typename container_type::const_reference    const_reference;
  typedef typename container_type::reference          reference;
  typedef typename container_type::pointer            pointer;
  typedef pointer                                     const_pointer;
//! @details May be used to examine, but not modify, elements in the deque.
  typedef typename container_type::const_iterator     const_iterator;
  typedef const_iterator                              iterator;
//! @details STL Container specifies that this is a signed integral type.
  typedef typename container_type::difference_type    difference_type;
//-------------------------------Constructors----------------------------------|
/** @brief Constructs a new priority deque.
//  @param comp Instance of comparison class.
//  @param seq Instance of container class.
//  @post Deque contains copies of all elements from @a sequence.
//
//  @par Complexity:
//    O(n) - Linear on the size of the deque.
//  @par Exception safety:
//    None.
*/
  priority_deque                      (void);
//! @overload
  explicit priority_deque             (Compare const &);
//! @overload
  priority_deque                      (Compare const &, Sequence const &);
#if (__cplusplus >= 201103L)
//! @overload
  explicit priority_deque             (Compare const &, Sequence &&);
#endif
/** @brief Constructs a new priority deque from a sequence of elements.
//  @param first,last Range of elements.
//  @param comp Instance of comparison class.
//  @param seq Instance of container class.
//  @post Deque contains copies of all elements in @a sequence (if specified)
//  and in the range [ @a first, @a last).
//
//  @par  Complexity:
//    O(n) - Linear on the size of the deque.
//  @par  Exception safety:
//    None.
*/
#if (__cplusplus >= 201103L)
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       Compare const & =Compare(),
                                       Sequence && =Sequence());
//! @overload
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       Compare const &,
                                       Sequence const &);
#else
  template <typename InputIterator>
  priority_deque                      (InputIterator first, InputIterator last,
                                       Compare const & =Compare(),
                                       Sequence const & =Sequence());
#endif
//-----------------------------Restricted Access-------------------------------|
/** @brief Copies an element into the priority deque.
//  @param value Element to insert into the priority deque.
//  @post Priority deque contains @a value or a copy of @a value.
//  @post All iterators and references are invalidated.
//
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Strong - If an exception is thrown, the deque is restored to the state it
//  had before push was called.
*/
  void                    push        (value_type const &);
#if (__cplusplus >= 201103L)
//!@overload
  void                    push        (value_type &&);

/** @brief Emplaces an element into the priority deque.
//  @param  args Arguments for the element to create in the the priority deque.
//  @post Priority deque contains an element constructed with arguments @a args.
//  @post All iterators and references are invalidated.
//
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Strong - If an exception is thrown, the deque is restored to the state it
//  had before emplace was called.
*/
  template<typename... Args>
  void                    emplace     (Args &&...);
#endif

//! @{
/** @brief Accesses a maximal element in the deque.
//  @return Const reference to a maximal element in the priority deque.
//  @pre  Priority deque contains one or more elements.
//  @see  minimum, pop_maximum
//
//  @par  Complexity:
//    O(1) - Does not on the size of the deque.
//  @par  Exception safety:
//    No-throw, if deque contains at least 1 element.
*/
  const_reference         maximum     (void) const;
/** @brief Accesses a minimal element in the deque.
//  @return Const reference to a minimal element in the priority deque.
//  @pre  Priority deque contains one or more elements.
//  @see  maximum, pop_minimum
//
//  @par  Complexity:
//    O(1) - Does not on the size of the deque.
//  @par  Exception safety:
//    No-throw, if deque contains at least 1 element.
*/
  const_reference         minimum     (void) const;
//! @details Identical to std::priority_queue top(). @see @a maximum
  inline const_reference  top         (void) const  { return maximum(); }

//! @}
//! @{

/** @brief Removes a maximal element from the deque.
//  @pre  Priority deque contains one or more elements.
//  @post A minimal element has been removed from the priority deque.
//  @post All iterators and references are invalidated.
//  @see  minimum, pop_maximum
//
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Strong - If an exception is thrown, the deque is restored to the state it
//  had before pop_maximum was called.
*/
  void                    pop_maximum (void);

/** @brief Removes a minimal element from the deque.
//  @pre  Priority deque contains one or more elements.
//  @post A maximal element has been removed from the priority deque.
//  @post All iterators and references are invalidated.
//  @see  maximum, pop, pop_minimum
//
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Strong - If an exception is thrown, the deque is restored to the state it
//  had before pop_minimum was called.
*/
  void                    pop_minimum (void);
//! @details Identical to std::priority_queue pop(). @see @a pop_maximum
  inline void             pop         (void)        { pop_maximum(); }
//! @}

//--------------------------------Deque Size-----------------------------------|
//! @{
//! @brief Returns true if the priority deque is empty, false if it is not.
//! @par  Complexity:
//!   O(1) - Does not on the size of the deque.
//! @par  Exception safety:
//!   No-throw.
  inline bool             empty       (void) const  {return sequence_.empty();}
//! @brief Returns the number of elements in the priority deque.
//! @par  Complexity:
//!   O(1) - Does not on the size of the deque.
//! @par  Exception safety:
//!   No-throw.
  inline size_type        size        (void) const  {return sequence_.size(); }
//! @brief Returns the maximum number of elements that can be contained.
//! @par  Complexity:
//!   O(1) - Does not on the size of the deque.
//! @par  Exception safety:
//!   No-throw.
  inline size_type        max_size    (void) const  {
    return sequence_.max_size();
  }
//! @}

//--------------------------Whole-Deque Operations-----------------------------|
//! @brief Removes all elements from the priority deque.
  inline void             clear       (void);

/** @brief Moves the elements from this deque into another, and vice-versa.
//  @param other Priority deque with which to swap.
//  @post Deque contains the elements from @a source, and @a source contains the
//  elements from this deque.
//  @post All iterators and references are invalidated.
//  @note Sequence containers are required to have swap functions.
//  @par  Complexity:
//    O(1) - Does not on the size of the deque.
//  @par  Exception safety:
//    No-throw.
*/
  void                    swap        (priority_deque<Type, Sequence,Compare>&);

//!@{
/** @brief Merges a sequence of elements into the priority deque.
//  @param first,last Input iterators bounding the range [ @a first, @a last)
//  @post Priority deque contains its original elements, and copies of those in
//  the range.
//  @post All iterators and references are invalidated.
//
//  @par Complexity:
//    O(n) - Linear on the size of the deque.
//  @par Exception safety:
//    Basic; invariants are maintained. Note that elements may be lost during
//    exception handling.
*/
  template <typename InputIterator>
  void                    insert      (InputIterator first, InputIterator last);
//! @brief Merges a sequence of elements into the priority deque.
//! @overload
  template <typename InputIterator>
  void                    merge       (InputIterator first, InputIterator last)
  {
    insert(first, last);
  }
//! @brief Merges a container's elements into the priority deque.
//! @overload
  template <typename SourceContainer>
  inline void             merge       (SourceContainer const & source)
  {
    insert(source.begin(), source.end());
  }
//!@}

//-------------------------------Random Access---------------------------------|
//!@{
//! @brief Returns a const iterator at the beginning of the sequence.
//! @par  Complexity:
//!   O(1) - Does not depend on the size of the deque.
//! @par  Exception safety:
//!   No-throw.
  inline const_iterator   begin       (void) const  {return sequence_.begin();}
//! @brief Returns a const iterator past the end of the sequence.
//! @par  Complexity:
//!   O(1) - Does not depend on the size of the deque.
//! @par  Exception safety:
//!   No-throw.
  inline const_iterator   end         (void) const  { return sequence_.end(); }

/** @brief Modifies a specified element of the deque.
//  @param  random_it A valid iterator in the range [begin, end).
//  @param  value The new value.
//  @pre  Priority deque contains one or more elements.
//  @post The element at @a random_it is set to @a value.
//  @post All iterators and references are invalidated.
//  @see erase
//
//  Elements within the deque may be unordered.
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Basic if const reference is passed, strong if rvalue reference is passed.
*/
  void                    update      (const_iterator, const value_type&);
#if (__cplusplus >= 201103L)
//!@overload
  void                    update      (const_iterator, value_type&&);
#endif

/** @brief Removes a specified element from the deque.
//  @param  random_it  An iterator in the range [begin, end)
//  @pre  Priority deque contains one or more elements.
//  @post The deque no longer contains the element previously at @a random_it.
//  @post All iterators and references are invalidated.
//  @see set
//  @par  Complexity:
//    O(log n) - Logarithmic on the size of the deque.
//  @par  Exception safety:
//    Strong - If an exception is thrown, the deque returned to its original
//  state.
*/
  void                    erase       (const_iterator);
//!@}

//---------------------------Boost.Heap Concepts-------------------------------|
// size() has constant complexity
  static const bool constant_time_size    = true;
// priority deque does not have ordered iterators
  static const bool has_ordered_iterators = false;
// priority deque is efficiently mergeable
  static const bool is_mergable           = true;
// priority deque does not have a stable heap order
  static const bool is_stable             = false;
// priority deque does not have a reserve() member
  static const bool has_reserve           = false;

//--------------------------------Protected------------------------------------|
 protected:
  inline        container_type& sequence  (void)        { return sequence_; }
  inline const  container_type& sequence  (void) const  { return sequence_; }
  inline        value_compare&  compare   (void)        { return compare_; }
  inline const  value_compare&  compare   (void) const  { return compare_; }
//---------------------------------Private-------------------------------------|
 private:
  void pop_back_or_rollback (void);
  Sequence sequence_;
  Compare compare_;
};

//-------------------------------Constructors----------------------------------|
//----------------------------Default Constructor------------------------------|
template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (void)
  : sequence_(), compare_()
{
//  Note: A "Container" is required to be empty when constructed.
}

template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (C const & comp)
  : sequence_(), compare_(comp)
{
//  Note: A "Container" is required to be empty when constructed.
}

template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (C const & comp, S const & seq)
  : sequence_(seq), compare_(comp)
{
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
priority_deque<T, S, C>::priority_deque (const C& comp, S&& seq)
  : sequence_(std::move(seq)), compare_(comp)
{
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#endif

//---------------------------Create from Iterators-----------------------------|
template <typename T, typename S, typename C>
template <typename InputIterator>
priority_deque<T, S, C>::priority_deque (InputIterator first,InputIterator last,
                                         C const & comp, S const & seq)
: sequence_(seq), compare_(comp)
{
  sequence_.insert(sequence_.end(), first, last);
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#if (__cplusplus >= 201103L)
template <typename T, typename S, typename C>
template <typename InputIterator>
priority_deque<T, S, C>::priority_deque (InputIterator first,InputIterator last,
                                         const C& comp, S&& seq)
: sequence_(std::move(seq)), compare_(comp)
{
  sequence_.insert(sequence_.end(), first, last);
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
}
#endif

//-----------------------------Restricted Access-------------------------------|
//------------------------------Insert / Emplace-------------------------------|
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::push (value_type const & value) {
  sequence_.push_back(value);

  struct RAIIGuard
  {
    Sequence * ptr;
    ~RAIIGuard (void)
    {
      if (ptr)
        ptr->pop_back();
    }
  } guard;
#if (__cplusplus >= 201103L)
  guard.ptr = std::addressof(sequence_);
#else
  guard.ptr = &sequence_;
#endif
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
#if (__cplusplus >= 201103L)
  guard.ptr = nullptr;
#else
  guard.ptr = NULL;
#endif
}
#if (__cplusplus >= 201103L)
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::push (value_type&& value) {
  sequence_.push_back(std::move(value));
  struct RAIIGuard
  {
    Sequence * ptr;
    ~RAIIGuard (void)
    {
      if (ptr)
        ptr->pop_back();
    }
  } guard;
  guard.ptr = std::addressof(sequence_);
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
  guard.ptr = nullptr;
}

template <typename T, typename Sequence, typename Compare>
template<typename... Args>
void priority_deque<T, Sequence, Compare>::emplace (Args&&... args) {
  sequence_.emplace_back(std::forward<Args>(args)...);
  struct RAIIGuard
  {
    Sequence * ptr;
    ~RAIIGuard (void)
    {
      if (ptr)
        ptr->pop_back();
    }
  } guard;
  guard.ptr = std::addressof(sequence_);
  heap::push_interval_heap(sequence_.begin(), sequence_.end(), compare_);
  guard.ptr = nullptr;
}
#endif

//---------------------------Observe Maximum/Minimum---------------------------|
template <typename T, typename Sequence, typename Compare>
inline typename priority_deque<T, Sequence, Compare>::const_reference
  priority_deque<T, Sequence, Compare>::maximum  (void) const
{
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no maximal element. Reference undefined.");
  const_iterator it = sequence_.begin() + 1;
  return (it == sequence_.end()) ? sequence_.front() : *it;
}

template <typename T, typename Sequence, typename Compare>
typename priority_deque<T, Sequence, Compare>::const_reference
  priority_deque<T, Sequence, Compare>::minimum  (void) const
{
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no minimal element. Reference undefined.");
  return sequence_.front();
}
//---------------------------Remove Maximum/Minimum----------------------------|
template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_minimum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no maximal element. Removal impossible.");
  heap::pop_interval_heap_min(sequence_.begin(), sequence_.end(), compare_);
  pop_back_or_rollback();
}

template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_maximum (void) {
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT(!empty(),
    "Empty priority deque has no minimal element. Removal undefined.");
  heap::pop_interval_heap_max(sequence_.begin(), sequence_.end(), compare_);
  pop_back_or_rollback();
}

//--------------------------Whole-Deque Operations-----------------------------|
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::clear (void) {
  sequence_.clear();
}
//-----------------------------------Merge-------------------------------------|
template <typename T, typename S, typename C>
template <typename InputIterator>
void priority_deque<T, S, C>::insert (InputIterator first, InputIterator last) {
  sequence_.insert(sequence_.end(), first, last);
  struct RAIIGuard
  {
    container_type * seq_;
#if (__cplusplus >= 201103L)
    RAIIGuard (container_type & seq) noexcept : seq_(std::addressof(seq)) {}
#else
    RAIIGuard (container_type & seq) : seq_(&seq) {}
#endif
    RAIIGuard (RAIIGuard const &);
    RAIIGuard & operator= (RAIIGuard const &);
    ~RAIIGuard (void)
    {
//    A heap-making failure is extremely difficult to resolve reliably. In
//  particular, we no longer know which elements are new elements. To maintain
//  the heap invariant, we may clear the sequence.
      if (seq_)
        seq_->clear();
    }
  } guard (sequence_);
  heap::make_interval_heap(sequence_.begin(), sequence_.end(), compare_);
#if (__cplusplus >= 201103L)
  guard.seq_ = nullptr;
#else
  guard.seq_ = NULL;
#endif
}

//----------------------------Swap Specialization------------------------------|
template <typename T, typename S, typename C>
inline void priority_deque<T, S, C>::swap (priority_deque<T, S, C>& other) {
  using std::swap;

  if (this != &other) {
    swap(compare_, other.compare_);
    sequence_.swap(other.sequence_);
  }
}

template <typename T, typename S, typename C>
inline void swap (priority_deque<T, S, C>& deque1,
                  priority_deque<T, S, C>& deque2)
{
  deque1.swap(deque2);
}

//---------------------------Random-Access Mutators----------------------------|
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::update (const_iterator random_it,
                                      const value_type& value)
{
  const difference_type ind = random_it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= ind) &&
    (ind < end() - begin()), "Iterator out of bounds; can't set element.");
//  Providing the strong guarantee would require saving a copy.
  *(sequence_.begin() + ind) = value;
  heap::update_interval_heap(sequence_.begin(),sequence_.end(),ind,compare_);
}
#if (__cplusplus >= 201103L)
//  Move-enabled function provides strong exception-safety guarantee.
template <typename T, typename S, typename C>
void priority_deque<T, S, C>::update (const_iterator random_it,
                                      value_type&& value)
{
  using std::swap;
  const difference_type ind = random_it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= ind) &&
    (ind < end() - begin()), "Iterator out of bounds; can't set element.");
//  Providing the strong guarantee would require saving a copy.
  typedef heap::interval_heap_internal::RAIISwapper<T> guard_t;
  guard_t scope_guard (std::addressof(*(sequence_.begin() + ind)), std::addressof(value));
  swap(*scope_guard.ptr1_, *scope_guard.ptr2_);
  heap::update_interval_heap(sequence_.begin(),sequence_.end(),ind,compare_);
  scope_guard.disable();
}
#endif

template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::erase (const_iterator random_it) {
  const difference_type index = random_it - begin();
  BOOST_CONTAINER_PRIORITY_DEQUE_ASSERT((0 <= index) &&
    (index < end() - begin()), "Iterator out of bounds; can't erase element.");
//  Move the element pointed to by random_it to the end of the sequence.
  heap::pop_interval_heap(sequence_.begin(), sequence_.end(), index,compare_);
  pop_back_or_rollback();
}

template <typename T, typename Sequence, typename Compare>
void priority_deque<T, Sequence, Compare>::pop_back_or_rollback (void)
{
  struct RAIIGuard
  {
    Sequence * seq_;
    Compare & comp_;
    RAIIGuard (Sequence & seq, Compare & comp)
#if (__cplusplus >= 201103L)
      noexcept : seq_(std::addressof(seq)), comp_(comp)
#else
      : seq_(&seq), comp_(comp)
#endif
    {
    }
    RAIIGuard (RAIIGuard const &);
    RAIIGuard & operator= (RAIIGuard const &);
    ~RAIIGuard (void)
    {
//  If pop_back failed, restore the heap property.
      if (seq_)
        heap::push_interval_heap(seq_->begin(), seq_->end(), comp_);
    }
  } guard (sequence_, compare_);
//  Remove the (moved) element.
  sequence_.pop_back();
#if (__cplusplus >= 201103L)
  guard.seq_ = nullptr;
#else
  guard.seq_ = NULL;
#endif
}

} //  Namespace boost::container
} //  Namespace boost

#endif
