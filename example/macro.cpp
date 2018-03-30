//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>

#include "te.hpp"

#define REQUIRES(R, name, ...)                                     \
  R {                                                              \
    return ::te::call<R>(                                          \
        [](auto&& self, auto&&... args) {                          \
          return self.name(std::forward<decltype(args)>(args)...); \
        },                                                         \
        *this, __VA_ARGS__);                                       \
  }

struct Addable {
  constexpr auto add(int i) -> REQUIRES(int, add, i);
  constexpr auto add(int a, int b) -> REQUIRES(int, add, a, b);
};

class Calc {
 public:
  constexpr auto add(int i) { return i; }
  constexpr auto add(int a, int b) { return a + b; }
};

int main() {
  te::poly<Addable> addable{Calc{}};
  assert(3 == addable.add(3));
  assert(3 == addable.add(1, 2));
}
