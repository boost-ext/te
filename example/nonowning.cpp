//
// Copyright (c) 2018-2019 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>
#include <iostream>

#include "boost/te.hpp"

namespace te = boost::te;

struct Drawable : te::poly<Drawable, te::non_owning_storage> {
  using te::poly<Drawable, te::non_owning_storage>::poly;

  void draw(std::ostream &out) {
    te::call([](auto &self, auto &out) { self.draw(out); }, *this, out);
  }
};

struct Square {
  int value;
  void draw(std::ostream &out) {
    out << "Square";
    value += 1;
  }
};

struct Circle {
  void draw(std::ostream &out) { out << "Circle"; }
};

void draw(Drawable drawable) { drawable.draw(std::cout); }

int main() {
  Circle c{};
  Square s{};

  draw(c);  // prints Circle
  draw(s);  // prints Square

  assert(1 == s.value);
}
