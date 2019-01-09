//
// Copyright (c) 2018-2019 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <experimental/type_traits>
#include <iostream>

#include "boost/te.hpp"

namespace te = boost::te;

void draw(struct Circle const &, std::ostream &);

struct Drawable {
  void draw(std::ostream &out) const {
    te::call(
        [](auto const &self, auto &out) {
          if constexpr (std::experimental::is_detected<
                            drawable_t, decltype(self), decltype(out)>{}) {
            self.draw(out);
          } else {
            ::draw(self, out);
          }
        },
        *this, out);
  }

 private:
  template <class T, class... Ts>
  using drawable_t = decltype(std::declval<T>().draw(std::declval<Ts>()...));
};

struct Square {
  void draw(std::ostream &out) const { out << "Member Square"; }
};

struct Circle {};

void draw(Circle const &, std::ostream &out) { out << "Non-member Circle"; }

void draw(te::poly<Drawable> const &drawable) { drawable.draw(std::cout); }

int main() {
  draw(Circle{});  // prints Non-member Circle
  draw(Square{});  // prints Member Square
}
