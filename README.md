Priority-Deque

==============



This project is my implementation of a priority deque (double-ended priority queue) as a C++ class template.

Goals:
* Interface similar to STL.
  * Status: Complete.
  * priority_deque implements the priority_queue interface.
  * Additional functions, for double-ended access and random-access among others, are in STL style.
* Interface in Boost style.
  * Status: Incomplete.
* Performance competitive with STL's priority_queue.
  * Status: Very close.
  * Benchmarks indicate ~20% slower performance in push, ~5% slower in pop. (GCC, -O3)
  * Bulk-loading is threaded, with the following benchmarked performance ratios (deque time / queue time, smaller is better):
    * 1.228 (1 CPU core)
    * 0.874 (2 CPU cores)
    * 0.545 (4 CPU cores)
* Provide the strongest exception-safety guarantee possible without compromising performance.
  * Status: Complete.
  * Most operations provide the strong (rollback) exception-safety guarantee if comparison throws.
  * Bulk-loading operations cannot provide this guarantee without compromising performance.
  * The random-access update operation can only provide this guarantee by copying an element or using move semantics.
* Provide thread safety, if possible.
  * Status: Complete.
  * Functions have no side-effects besides the obvious (deque or interval heap may be altered), and can safely be called in threads, though a single queue should not be accessed by multiple threads simultaneously. Multiple reads are okay; simultaneous read-write and write-write may cause race conditions.
  * Automatic locking via mutex crippled performance, and has been removed.
