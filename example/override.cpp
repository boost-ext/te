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

namespace v1 {
struct Drawable {
  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &&... args) { self.draw(args...); },
             *this, out, "v1");
  }
};
}  // namespace v1

namespace v2 {
struct Drawable : v1::Drawable {
  Drawable() { te::extends<v1::Drawable>(*this); }
};
}  // namespace v2

namespace v3 {
struct Drawable : v2::Drawable {
  Drawable() { te::extends<v2::Drawable>(*this); }

  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &&... args) { self.draw(args...); },
             *this, out, "v3");
  }

  void draw(std::ostream &out, int minor) const {
    te::call([](auto const &self, auto &&... args) { self.draw(args...); },
             *this, out, "v3." + std::to_string(minor));
  }
};
}  // namespace v3

struct Square {
  void draw(std::ostream &out, const std::string &v) const {
    out << v << "::Square";
  }
};

struct Circle {
  void draw(std::ostream &out, const std::string &v) const {
    out << v << "::Circle";
  }
};

template <class T, class... Ts>
void draw(te::poly<T> const &drawable, const Ts... args) {
  drawable.draw(std::cout, args...);
}

int main() {
  draw<v1::Drawable>(Circle{});  // prints v1::Circle
  draw<v1::Drawable>(Square{});  // prints v1::Square

  draw<v2::Drawable>(Circle{});  // prints v1::Circle
  draw<v2::Drawable>(Square{});  // prints v1::Square

  draw<v3::Drawable>(Circle{});     // prints v3::Circle
  draw<v3::Drawable>(Circle{}, 1);  // prints v3.1::Circle
  draw<v3::Drawable>(Square{});     // prints v3::Square
  draw<v3::Drawable>(Square{}, 2);  // prints v3.2::Square
}
