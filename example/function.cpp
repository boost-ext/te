//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>

#include "te.hpp"

template <class>
struct Function;

template <class R, class... Ts>
struct Function<R(Ts...)> {
  constexpr inline auto operator()(Ts... args) {
    return te::call<R>([](auto &self, Ts... args) { return self(args...); },
                       *this, args...);
  }

  // Option 1 - Explicit requirements for class templates
  template <class T>
  auto requires__() -> decltype(&T::operator());
};

// Option 2 - Explicit template instantiation
// template struct Function<int(int)>;

int main() {
  // Option 1 or 2 is required on some compilers
  te::poly<Function<int(int)>> f{[](int i) { return i; }};
  assert(42 == f(42));
}
