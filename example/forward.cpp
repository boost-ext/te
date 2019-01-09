//
// Copyright (c) 2018-2019 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>

#include "boost/te.hpp"

namespace te = boost::te;

// header
struct Drawable {
  void draw(std::ostream &out) const;
};

struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

struct Circle {
  void draw(std::ostream &out) const { out << "Circle"; }
};

// cpp
void draw(te::poly<Drawable> const &drawable) { drawable.draw(std::cout); }

void Drawable::draw(std::ostream &out) const {
  te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
}

// usage
int main() {
  draw(Circle{});  // prints Circle
  draw(Square{});  // prints Square
}
