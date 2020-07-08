# [Boost::ext].TE <a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a> <a href="https://travis-ci.org/boost-ext/te" target="_blank">![Build Status](https://img.shields.io/travis/boost-ext/te/master.svg?label=linux/osx)</a> <a href="https://codecov.io/gh/boost-ext/te" target="_blank">![Coveralls](https://codecov.io/gh/boost-ext/te/branch/master/graph/badge.svg)</a> <a href="http://github.com/boost-ext/te/issues" target="_blank">![Github Issues](https://img.shields.io/github/issues/boost-ext/te.svg)</a> <a href="https://godbolt.org/g/8YmMLX">![Try it online](https://img.shields.io/badge/try%20it-online-blue.svg)</a>

> Your C++17 **one header only** run-time polymorphism (type erasure) library with no dependencies, macros and limited boilerplate

* *Duck typing ([No inheritance](https://en.wikipedia.org/wiki/Duck_typing))*
* *Value semantics ([Value vs reference semantics](https://isocpp.org/wiki/faq/value-vs-ref-semantics))*
* *Automatic memory management (No need for `std::unique_ptr`, `std::shared_ptr`)*
* *Unified call syntax ([Uniform function call syntax](https://en.wikipedia.org/wiki/Uniform_Function_Call_Syntax))*
* *Performance ([Better inlining](https://godbolt.org/g/cEX1eL))*

### Quick start

> [Boost::ext].TE requires only one file. Get the latest header [here!](https://github.com/boost-ext/te/blob/master/include/boost/te.hpp)

```cpp
#include <boost/te.hpp> // Requires C++17 (Tested: GCC-6+, Clang-4.0+, Apple:Clang-9.1+)
namespace te = boost::te;
```

#### Erase it

```cpp
// Define interface of something which is drawable
struct Drawable {
  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
  }
};

// Define implementation which is drawable (No inheritance)
struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

// Define other implementation which is drawable (No inheritance)
struct Circle {
  void draw(std::ostream &out) const { out << "Circle"; }
};

// Define object which can hold drawable objects
void draw(te::poly<Drawable> const &drawable) { drawable.draw(std::cout); }

int main() {
  // Call draw polymorphically (Value semantics / Small Buffer Optimization)
  draw(Circle{}); // prints Circle
  draw(Square{}); // prints Square
}
```

#### Alternatively, erase + declare it

```cpp
struct Drawable : te::poly<Drawable> {
  using te::poly<Drawable>::poly;

  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
  }
};

void draw(Drawable const &drawable) { drawable.draw(std::cout); }

int main() {
  draw(Circle{}); // prints Circle
  draw(Square{}); // prints Square
}
```

#### Store it

```cpp
int main() {
  std::vector<te::poly<Drawable>> drawables{};

  drawables.push_back(Square{});
  drawables.push_back(Circle{});

  for (const auto &drawable : drawables) {
    drawable.draw(std::cout); // prints Square Circle
  }
}
```

#### Overload it

```cpp
struct Addable {
  constexpr auto add(int i)        { return te::call<int>(add_impl, *this, i); }
  constexpr auto add(int a, int b) { return te::call<int>(add_impl, *this, a, b); }

private:
  static constexpr auto add_impl = [](auto &self, auto... args) {
    return self.add(args...);
  };
};

class Calc {
public:
  constexpr auto add(int i)        { return i; }
  constexpr auto add(int a, int b) { return a + b; }
};

int main() {
  te::poly<Addable> addable{Calc{}};
  assert(3 == addable.add(3));
  assert(3 == addable.add(1, 2));
}
```

#### Override it

```cpp
namespace v1 {
  struct Drawable {
    void draw(std::ostream &out) const {
      te::call([](auto const &self, auto &&... args) { self.draw(args...); }, *this, out, "v1");
    }
  };
} // namespace v1

namespace v2 {
  struct Drawable : v1::Drawable {
    Drawable() { te::extends<v1::Drawable>(*this); }

    // override
    void draw(std::ostream &out) const {
      te::call([](auto const &self, auto &&... args) { self.draw(args...); }, *this, out, "v2");
    }

    // extend/overload
    void draw(std::ostream& out, int minor) const {
      te::call([](auto const &self, auto &&... args) { self.draw(args...); },
        *this, out, "v2." + std::to_string(minor));
    }
  };
} // namespace v2

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
  draw<v1::Drawable>(Circle{});    // prints v1::Circle
  draw<v1::Drawable>(Square{});    // prints v1::Square

  draw<v2::Drawable>(Circle{});    // prints v2::Circle
  draw<v2::Drawable>(Circle{}, 1); // prints v2.1::Circle
  draw<v2::Drawable>(Square{});    // prints v2::Square
  draw<v2::Drawable>(Square{}, 2); // prints v2.2::Square
}
```

#### Conceptify it (Requires C++20)

```cpp
struct Drawable {
  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out)
      -> decltype(self.draw(out)) { self.draw(out); }, *this, out);
  }
};

struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

struct Circle {
  // void draw(std::ostream &out) const { out << "Circle"; }
};

template<te::conceptify<Drawable> TDrawable>
void draw(TDrawable const &drawable) { drawable.draw(std::cout); }

int main() {
  {
    te::var<Drawable> drawable = Square{};
    drawable.draw(std::cout);// prints Square
  }

  {
    // te::var<Drawable> drawable = Square{}; // error: deduced initializer does not
    // drawable.draw(std::cout);              //        satisfy placeholder constraints (draw)
  }

  {
    auto drawable = Square{};
    draw(drawable);  // prints Square
  }

  {
    // auto drawable = Circle{}; // error: constraints not satisifed (draw)
    // draw(drawable);
  }
}
```

#### Unify it

```cpp
void draw(struct Circle const&, std::ostream&);

struct Drawable {
  void draw(std::ostream &out) const {
    te::call(
      [](auto const &self, auto &out) {
        if constexpr(std::experimental::is_detected<drawable_t, decltype(self), decltype(out)>{}) {
          self.draw(out);
        } else {
          ::draw(self, out);
        }
      }, *this, out
    );
  }

 private:
  template<class T, class... Ts>
  using drawable_t = decltype(std::declval<T>().draw(std::declval<Ts>()...));
};

struct Square {
  void draw(std::ostream &out) const { out << "Member Square"; }
};

struct Circle { };

void draw(Circle const&, std::ostream& out) {
  out << "Non-member Circle";
}

void draw(te::poly<Drawable> const &drawable) { drawable.draw(std::cout); }

int main() {
  draw(Circle{});  // prints Non-member Circle
  draw(Square{});  // prints Member Square
}
```

#### Forward it

```cpp
// header
struct Drawable {
  void draw(std::ostream &out) const;
};

void draw(te::poly<Drawable> const &);

struct Square {
  void draw(std::ostream &out) const { out << "Square"; }
};

struct Circle {
  void draw(std::ostream &out) const { out << "Circle"; }
};

// cpp
void draw(te::poly<Drawable> const &drawable) { drawable.draw(std::cout); }

void Drawable::draw(std::ostream& out) const {
  te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
}

// uasage
int main() {
  draw(Circle{});  // prints Circle
  draw(Square{});  // prints Square
}
```

#### Call it

```cpp
template <class> struct Function;

template <class R, class... Ts> struct Function<R(Ts...)> {
  constexpr inline auto operator()(Ts... args) {
    return te::call<R>([](auto &self, Ts... args) { return self(args...); }, *this, args...);
  }

  // Option 1 - Explicit requirements for class templates
  // template<class T>
  // auto requires__() -> decltype(&T::operator());
};

// Option 2 - Explicit template instantiation
// template struct Function<int(int)>;

int main() {
  // Option 1 or 2 is required on some compilers
  te::poly<Function<int(int)>> f{[](int i) { return i; }};
  assert(42 == f(42));
}
```

#### Customize it

```cpp
int main() {
  te::poly<Drawable,
           te::local_storage<16>, // te::dynamic_storage (default)
           te::static_vtable      // (default)
  > drawable{Circle{}};
  drawable.draw(std::cout); // prints Circle
}
```

#### Macro it?

```cpp
#define REQUIRES(R, name, ...) R {                             \
  return ::te::call<R>(                                        \
    [](auto&& self, auto&&... args) {                          \
      return self.name(std::forward<decltype(args)>(args)...); \
    }, *this, ## __VA_ARGS__                                   \
  );                                                           \
}

struct Addable {
  auto add(int i) -> REQUIRES(int, add, i);
  auto add(int a, int b) -> REQUIRES(int, add, a, b);
};

class Calc {
 public:
  auto add(int i) { return i; }
  auto add(int a, int b) { return a + b; }
};

int main() {
  te::poly<Addable> addable{Calc{}};
  assert(3 == addable.add(3));
  assert(3 == addable.add(1, 2));
}
```

#### Inject it ([[Boost::ext].DI](https://github.com/boost-ext/di))

```cpp
class Example {
 public:
  Example(Drawable const drawable, std::ostream& out)
    : drawable{drawable}, out{out}
  { }

  void draw() {
    drawable.draw(out);
  }

 private:
  Drawable const drawable;
  std::ostream& out;
};

int main() {
  const auto injector = di::make_injector(
    di::bind<Drawable>.to<Circle>(),
    di::bind<std::ostream>.to(std::cout)
  );

  injector.create<Example>().draw(); // prints Circle
}
```

#### Mock it ([GUnit](https://github.com/cpp-testing/GUnit))

```cpp
"should mock drawable object"_test = [] {
  GMock<Drawable> drawable;

  EXPECT_CALL(drawable, draw(std::cout));

  draw(drawable);
};
```

#### Similar libraries

* [Adobe.Poly](http://stlab.adobe.com/group__poly__related.html)
* [Boost.TypeErasure](https://www.boost.org/doc/libs/1_66_0/doc/html/boost_typeerasure.html)
* [Folly.Poly](https://github.com/facebook/folly/blob/master/folly/docs/Poly.md)
* [Dyno](https://github.com/ldionne/dyno)

---

**Disclaimer** `[Boost::ext].TE` is not an official Boost library.
