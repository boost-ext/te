//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <vector>

#include "te.hpp"

struct Drawable {
  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
  }
};

struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

struct Circle {
  void draw(std::ostream &out) const { out << "Circle"; }
};

struct Triangle {
  void draw(std::ostream &out) const { out << "Triangle"; }
};

int main() {
  std::vector<te::poly<Drawable>> drawables{};

  drawables.push_back(Square{});
  drawables.push_back(Circle{});
  drawables.push_back(Triangle{});

  for (const auto &drawable : drawables) {
    drawable.draw(std::cout);
  }
}
