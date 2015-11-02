Priority-Deque

==============



This header-only library is my implementation of a double-ended priority queue (a priority deque) as a C++ class template. The project started a few years ago, and is updated whenever I think of some way to improve it.

Goals:

Interface similar to STL's priority_queue. Status: Achieved. Converting from priority_queue to priority_deque is as simple as changing two letters ("qu" to "de").

Performance competitive with STL's priority_queue. Status: Achieved. Benchmarks indicate ~20% slower performance in push, ~5% slower in pop. (GCC, -O3)

Provide the strongest exception-safety and thread-safety guarantees possible without compromising performance. Status: Probably achieved. Strengthening the guarantees currently given would almost certainly require crippling the performance.
