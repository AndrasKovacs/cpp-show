cpp-show
========

Typeclass (or Concept) style implementation for generic and extensible conversion to `std::string`. 

You can add new instances by specializing the `Show` class to a type (and providing an implementation for the static `show` method. Forward iterable types may inherit the show method from the default implementation class `Show_fwd_iterable`.

Included here are instances for primitive types, pairs, tuples, vectors, strings and C-style arrays and strings.
