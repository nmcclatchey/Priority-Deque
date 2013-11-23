#ifndef PRIORITY_DEQUE_VERIFY_HPP_
#define PRIORITY_DEQUE_VERIFY_HPP_

#include "priority_deque.hpp"

//    Thouroughly checks a priority deque for heap corruption and returns the
//  first instance of corruption (or the end iterator if there is none).
//    Note: Assumes comparison object is default-constructed.
template <typename T, typename S, typename C>
typename boost::container::priority_deque<T, S, C>::const_iterator
  is_valid_until (const boost::container::priority_deque<T, S, C>& pd)
{
  typedef boost::container::priority_deque<T, S, C> pd_type;
  typedef typename pd_type::difference_type difference_type;

  C compare;
  typename pd_type::const_iterator it = pd.begin();
  const difference_type size = pd.end() - it;
  for (difference_type n = 0; n < size; ++n) {
    if (n > 1) {
      int parent = ((n / 2 - 1) | 1) ^ 1 ^ (n & 1);
      if (n & 1) {
        if (compare(it[n], it[parent]))
          return it + n;
      } else {
        if (compare(it[parent], it[n]) || compare(it[n], it[parent | 1]))
          return it + n;
      }
    }
    if ((n & 1) && compare(it[n ^ 1], it[n]))
      return it + n;
  }
  return pd.end();
}



#endif // PRIORITY_DEQUE_VERIFY_HPP_
