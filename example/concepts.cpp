//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#if defined(__cpp_concepts)
#include <iostream>

#include "te.hpp"

struct Drawable {
  void draw(std::ostream &out) const {
    te::call(
        [](auto const &self, std::ostream &out) -> decltype(self.draw(out)) {
          self.draw(out);
        },
        *this, out);
  }
};

struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

struct Circle {
  void draw(std::ostream &out) const { out << "Circle"; }
};

template <class TDrawable>
requires te::conceptify<Drawable, TDrawable> void draw(
    TDrawable const &drawable) {
  drawable.draw(std::cout);
}

int main() {
  {
    te::var<Drawable> drawable = Square{};
    drawable.draw(std::cout);  // prints Square
  }

  {
    te::var<Drawable> drawable = Circle{};
    drawable.draw(std::cout);  // prints Circle
  }

  {
    auto drawable = Square{};
    draw(drawable);  // prints Square
  }

  {
    auto drawable = Circle{};
    draw(drawable);  // prints Circle
  }
}
#else
int main() {}
#endif
