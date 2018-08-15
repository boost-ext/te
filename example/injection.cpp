//
// Copyright (c) 2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#if __has_include(<boost/di.hpp>)
#include <boost/di.hpp>
#include <iostream>

#include "boost/te.hpp"

namespace te = boost::te;

struct Drawable : te::poly<Drawable> {
  using te::poly<Drawable>::poly;

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

class Example {
 public:
  Example(Drawable const drawable, std::ostream &out)
      : drawable{drawable}, out{out} {}

  void draw() { drawable.draw(out); }

 private:
  Drawable const drawable;
  std::ostream &out;
};

int main() {
  namespace di = boost::di;

  // clang-format off
  const auto injector = di::make_injector(
    di::bind<Drawable>.to<Circle>(),
    di::bind<std::ostream>.to(std::cout)
  );
  // clang-format on

  injector.create<Example>().draw();  // prints Circle
}
#else
int main() {}
#endif
