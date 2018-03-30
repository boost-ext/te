//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>

#include "te.hpp"

struct Addable {
  constexpr auto add(int i) { return te::call<int>(add_impl, *this, i); }
  constexpr auto add(int a, int b) {
    return te::call<int>(add_impl, *this, a, b);
  }

 private:
  static constexpr auto add_impl = [](auto &self, auto... args) {
    return self.add(args...);
  };
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
