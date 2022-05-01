//
// Copyright (c) 2018-2019 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <sstream>
#include <type_traits>
#include <vector>

#include "boost/te.hpp"
#include "common/test.hpp"

namespace te = boost::te;

test should_set_get_mappings = [] {
  struct B {};
  te::detail::mappings<class A, 0>::set<B>();
  static_assert(
      std::is_same<B, decltype(get(te::detail::mappings<class A, 0>{}))>{});

  struct C {};
  te::detail::mappings<class A, 1>::set<C>();
  static_assert(
      std::is_same<C, decltype(get(te::detail::mappings<class A, 1>{}))>{});
};

test should_return_mappings_size = [] {
  static_assert(
      0 ==
      te::detail::mappings_size<class Size, std::integral_constant<int, 0>>());
  te::detail::mappings<class Size, 1>::set<std::integral_constant<int, 1>>();
  te::detail::mappings<class Size, 2>::set<std::integral_constant<int, 2>>();
  te::detail::mappings<class Size, 3>::set<std::integral_constant<int, 3>>();
  static_assert(
      3 ==
      te::detail::mappings_size<class Size, std::integral_constant<int, 1>>());
};

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

test should_erase_the_call = [] {
  te::poly<Drawable> drawable{Square{}};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    drawable = Circle{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

struct DrawableDeclare : te::poly<DrawableDeclare> {
  using te::poly<DrawableDeclare>::poly;

  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
  }
};

test should_erase_and_declare_call = [] {
  DrawableDeclare drawable{Square{}};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    drawable = Circle{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

struct DrawableMacro {
  auto draw(std::ostream &out) const -> REQUIRES(void, draw, out);
};

test should_erase_the_call_using_macro = [] {
  te::poly<DrawableMacro> drawable{Square{}};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    drawable = Circle{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

struct DrawableDeclareCustomStorage
    : te::poly<DrawableDeclare, te::local_storage<16>> {
  using te::poly<DrawableDeclare, te::local_storage<16>>::poly;

  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &out) { self.draw(out); }, *this, out);
  }
};

test should_erase_and_declare_with_custom_storage_call = [] {
  DrawableDeclareCustomStorage drawable{Square{}};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    drawable = Circle{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

test should_reassign = [] {
  te::poly<Drawable> drawable{Circle{}};
  drawable = Square{};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Square" == str.str());
  }
};

struct CircleNoncopyable {
  CircleNoncopyable() = default;
  CircleNoncopyable(const CircleNoncopyable &) = delete;
  CircleNoncopyable &operator=(const CircleNoncopyable &) = delete;
  CircleNoncopyable(CircleNoncopyable &&) = default;
  CircleNoncopyable &operator=(CircleNoncopyable &&) = default;
  ~CircleNoncopyable() noexcept = default;

  void draw(std::ostream &out) const { out << "Circle"; }
};

struct SquareNoncopyable {
  SquareNoncopyable() = default;
  SquareNoncopyable(const SquareNoncopyable &) = delete;
  SquareNoncopyable &operator=(const SquareNoncopyable &) = delete;
  SquareNoncopyable(SquareNoncopyable &&) = default;
  SquareNoncopyable &operator=(SquareNoncopyable &&) = default;
  ~SquareNoncopyable() noexcept = default;

  void draw(std::ostream &out) const { out << "Square"; }
};

struct TriangleCopyable {
  void draw(std::ostream &out) const { out << "Triangle"; }
};

test should_support_movable_only_types = [] {
  te::poly<Drawable> drawable = CircleNoncopyable{};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }

  {
    std::stringstream str{};
    drawable = SquareNoncopyable{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  te::poly<Drawable> drawable2 = SquareNoncopyable{}; //need this as te::poly is not default constructible yet

  {
    try {
      /*! Try to copy. This should throw as SquareNoncopyable is non-copyable !*/
      drawable2 = drawable;
      expect(false);
    } catch(const std::exception&) {
      expect(true);
    }
  }

  drawable2 = TriangleCopyable{};

  {
    try {
      /*! Try to copy. This should succeed as TriangeCopyable is copyable !*/
      drawable = drawable2;
      expect(true);
    } catch (const std::exception &) {
      expect(false);
    }
  }
};

test should_support_movable_only_types_with_local_storage = [] {
  te::poly<Drawable, te::local_storage<32>> drawable = CircleNoncopyable{};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }

  {
    std::stringstream str{};
    drawable = SquareNoncopyable{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  te::poly<Drawable, te::local_storage<32>> drawable2 = SquareNoncopyable{}; //need this as te::poly is not default constructible yet

  {
    try {
      /*! Try to copy. This should throw as SquareNoncopyable is non-copyable !*/
      drawable2 = drawable;
      expect(false);
    } catch(const std::exception&) {
      expect(true);
    }
  }

  drawable2 = TriangleCopyable{};

  {
    try {
      /*! Try to copy. This should succeed as TriangeCopyable is copyable !*/
      drawable = drawable2;
      expect(true);
    } catch (const std::exception &) {
      expect(false);
    }
  }
};

test should_support_movable_only_types_with_sbo_storage = [] {
  te::poly<Drawable, te::sbo_storage<32>> drawable = CircleNoncopyable{};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }

  {
    std::stringstream str{};
    drawable = SquareNoncopyable{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  te::poly<Drawable, te::sbo_storage<32>> drawable2 = SquareNoncopyable{}; //need this as te::poly is not default constructible yet

  {
    try {
      /*! Try to copy. This should throw as SquareNoncopyable is non-copyable !*/
      drawable2 = drawable;
      expect(false);
    } catch(const std::exception&) {
      expect(true);
    }
  }

  drawable2 = TriangleCopyable{};

  {
    try {
      /*! Try to copy. This should succeed as TriangeCopyable is copyable !*/
      drawable = drawable2;
      expect(true);
    } catch (const std::exception &) {
      expect(false);
    }
  }
};

test should_support_movable_only_types_with_shared_storage = [] {
  te::poly<Drawable, te::shared_storage> drawable = CircleNoncopyable{};

  {
    std::stringstream str{};
    drawable.draw(str);
    expect("Circle" == str.str());
  }

  {
    std::stringstream str{};
    drawable = SquareNoncopyable{};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  te::poly<Drawable, te::shared_storage> drawable2 = CircleNoncopyable{}; //need to assign to something as te::poly is not default constructible yet.

  {
    try {
      /*! Copying is fine with shared_storage !*/
      drawable2 = drawable;
      expect(true);
    } catch (const std::exception &) {
      expect(false);
    }
  }
};

test should_support_containers = [] {
  std::vector<te::poly<Drawable>> drawables{};
  drawables.reserve(42);

  drawables.push_back(Square{});
  drawables.push_back(Circle{});
  drawables.emplace_back(Triangle{});

  std::stringstream str{};
  for (const auto &drawable : drawables) {
    drawable.draw(str);
  }
  expect("SquareCircleTriangle" == str.str());
};

struct Addable {
  auto add(int i) {
    return te::call<int>(
        [](auto &self, auto... args) { return self.add(args...); }, *this, i);
  }
  auto add(int a, int b) {
    return te::call<int>(
        [](auto &self, auto... args) { return self.add(args...); }, *this, a,
        b);
  }
};

class Calc {
 public:
  constexpr explicit Calc(const int i = {}) : i_{i} {}

  constexpr auto add(int i) { return i + i_; }
  constexpr auto add(int a, int b) { return (a + b) + i_; }

 private:
  int i_{};
};

test should_support_overloads = [] {
  te::poly<Addable> addable{Calc{2}};
  expect(2 + 3 == addable.add(3));
  expect(2 + 3 == addable.add(1, 2));
};

namespace v1 {
struct Drawable {
  void draw(std::ostream &out) const {
    te::call([](auto const &self, auto &&... args) { self.draw(args...); },
             *this, out, "v1");
  }
};
}  //  namespace v1

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
};
}  // namespace v3

test should_support_overrides = [] {
  struct Square {
    void draw(std::ostream &out, const std::string &v) const {
      out << v << "::Square ";
    }
  };

  struct Circle {
    void draw(std::ostream &out, const std::string &v) const {
      out << v << "::Circle ";
    }
  };

  const auto draw = [](auto const &drawable, auto &str) { drawable.draw(str); };

  {
    std::stringstream str{};
    draw(te::poly<v1::Drawable>{Circle{}}, str);
    draw(te::poly<v1::Drawable>{Square{}}, str);
    expect("v1::Circle v1::Square " == str.str());
  }

  {
    std::stringstream str{};
    draw(te::poly<v2::Drawable>{Circle{}}, str);
    draw(te::poly<v2::Drawable>{Square{}}, str);
    expect("v1::Circle v1::Square " == str.str());
  }

  {
    std::stringstream str{};
    draw(te::poly<v3::Drawable>{Circle{}}, str);
    draw(te::poly<v3::Drawable>{Square{}}, str);
    expect("v3::Circle v3::Square " == str.str());
  }
};

template <class T>
struct DrawableT {
  void draw(T &out) const {
    te::call([](auto const &self, T &out) { self.draw(out); }, *this, out);
  }
};

template struct DrawableT<std::ostream>;
template struct DrawableT<std::stringstream>;

test should_support_templated_interfaces = [] {
  {
    std::stringstream str{};
    te::poly<DrawableT<std::ostream>> drawable{Square{}};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    te::poly<DrawableT<std::stringstream>> drawable{Circle{}};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

template <class T>
struct DrawableI {
  auto draw(T &out) const {
    te::call([](auto const &self, T &out) { self.draw(out); }, *this, out);
  }

  template <class U>
  auto requires__() -> decltype(&U::draw);
};

test should_support_templated_interfaces_using_requirements = [] {
  {
    std::stringstream str{};
    te::poly<DrawableI<std::ostream>> drawable{Square{}};
    drawable.draw(str);
    expect("Square" == str.str());
  }

  {
    std::stringstream str{};
    te::poly<DrawableI<std::stringstream>> drawable{Circle{}};
    drawable.draw(str);
    expect("Circle" == str.str());
  }
};

template <class>
struct Function;

template <class R, class... Ts>
struct Function<R(Ts...)> {
  constexpr inline R operator()(Ts... args) {
    return te::call<R>([](auto &self, Ts... args) { return self(args...); },
                       *this, args...);
  }
};

template struct Function<int(int)>;
template struct Function<int(int, int)>;

test should_support_function_lambda_expr = [] {
  {
    te::poly<Function<int(int)>> f{[](int i) { return i; }};
    expect(0 == f(0));
    expect(42 == f(42));
  }

  {
    te::poly<Function<int(int, int)>> f{[](int a, int b) { return a + b; }};
    expect(0 == f(0, 0));
    expect(3 == f(1, 2));
  }
};

class Ctor;
class CopyCtor;
class MoveCtor;
class Dtor;

struct Storage {
  template <class T>
  static auto &calls() {
    static auto calls = 0;
    return calls;
  }

  Storage() { ++calls<Ctor>(); }
  Storage(const Storage &) { ++calls<CopyCtor>(); }
  Storage(Storage &&) { ++calls<MoveCtor>(); }
  ~Storage() { ++calls<Dtor>(); }
};

test should_support_dynamic_storage = [] {
  Storage::calls<Ctor>() = 0;
  Storage::calls<CopyCtor>() = 0;
  Storage::calls<MoveCtor>() = 0;
  Storage::calls<Dtor>() = 0;

  {
    te::dynamic_storage storage{Storage{}};
  }

  expect(1 == Storage::calls<Ctor>());
  expect(0 == Storage::calls<CopyCtor>());
  expect(1 == Storage::calls<MoveCtor>());
  expect(2 == Storage::calls<Dtor>());
};

test should_support_local_storage = [] {
  Storage::calls<Ctor>() = 0;
  Storage::calls<CopyCtor>() = 0;
  Storage::calls<MoveCtor>() = 0;
  Storage::calls<Dtor>() = 0;

  {;
    te::local_storage<16> storage{Storage{}};
  }

  expect(1 == Storage::calls<Ctor>());
  expect(0 == Storage::calls<CopyCtor>());
  expect(1 == Storage::calls<MoveCtor>());
  expect(2 == Storage::calls<Dtor>());
};

test should_support_sbo_storage = [] {
  Storage::calls<Ctor>() = 0;
  Storage::calls<CopyCtor>() = 0;
  Storage::calls<MoveCtor>() = 0;
  Storage::calls<Dtor>() = 0;

  {;
    te::sbo_storage<16> storage{Storage{}};
  }

  expect(1 == Storage::calls<Ctor>());
  expect(0 == Storage::calls<CopyCtor>());
  expect(1 == Storage::calls<MoveCtor>());
  expect(2 == Storage::calls<Dtor>());
};

test should_support_shared_storage = [] {
  Storage::calls<Ctor>() = 0;
  Storage::calls<CopyCtor>() = 0;
  Storage::calls<MoveCtor>() = 0;
  Storage::calls<Dtor>() = 0;

  {;
    te::shared_storage storage{Storage{}};
  }

  expect(1 == Storage::calls<Ctor>());
  expect(0 == Storage::calls<CopyCtor>());
  expect(1 == Storage::calls<MoveCtor>());
  expect(2 == Storage::calls<Dtor>());
};

test should_support_custom_storage = [] {
  {
    te::poly<Addable> addable_def{Calc{}};
    expect(42 == addable_def.add(40, 2));

    te::poly<Addable> addable_copy_def{addable_def};
    expect(42 == addable_copy_def.add(40, 2));

    te::poly<Addable> addable_move_def{std::move(addable_def)};
    expect(42 == addable_move_def.add(40, 2));
  }

  {
    te::poly<Addable, te::local_storage<16>> addable_local{Calc{4}};
    expect(46 == addable_local.add(40, 2));

    te::poly<Addable, te::local_storage<16>> addable_copy_local{addable_local};
    expect(46 == addable_copy_local.add(40, 2));

    te::poly<Addable> addable_move_local{std::move(addable_local)};
    expect(46 == addable_move_local.add(40, 2));
  }

  {
    te::poly<Addable, te::sbo_storage<16>> addable_local{Calc{4}};
    expect(46 == addable_local.add(40, 2));

    te::poly<Addable, te::sbo_storage<16>> addable_copy_local{addable_local};
    expect(46 == addable_copy_local.add(40, 2));

    te::poly<Addable> addable_move_local{std::move(addable_local)};
    expect(46 == addable_move_local.add(40, 2));
  }

  {
    te::poly<Addable, te::shared_storage> addable_local{Calc{4}};
    expect(46 == addable_local.add(40, 2));

    te::poly<Addable, te::shared_storage> addable_copy_local{addable_local};
    expect(46 == addable_copy_local.add(40, 2));

    te::poly<Addable> addable_move_local{std::move(addable_local)};
    expect(46 == addable_move_local.add(40, 2));
  }
};

struct DrawableMutable : te::poly<DrawableMutable, te::non_owning_storage> {
  using te::poly<DrawableMutable, te::non_owning_storage>::poly;

  void draw(std::ostream &out) {
    te::call([](auto &self, auto &out) { self.draw(out); }, *this, out);
  }
};

struct SquareMutable {
  int value;

  void draw(std::ostream &out) {
    out << "Square Mutable";
    value += 1;
  }
};

test should_support_non_owning_storage = [] {
  const auto draw_mut = [](auto &drawable, auto &str) { drawable.draw(str); };
  {
    SquareMutable a{};
    std::stringstream str{};

    draw_mut(a, str);

    expect("Square Mutable" == str.str());
    expect(1 == a.value);
  }

  {
    SquareMutable b{};
    SquareMutable c{};
    std::stringstream str{};
    std::vector<DrawableMutable> drawables_mut{b, c};

    for (auto &drawable : drawables_mut) {
      draw_mut(drawable, str);
    }

    expect("Square MutableSquare Mutable" == str.str());
    expect(1 == b.value);
    expect(1 == c.value);
  }
};

#if (__cpp_concepts)
struct DrawableConcept {
  void draw(std::ostream &out) const {
    te::call([](auto const &self,
                auto &out) -> decltype(self.draw(out)) { self.draw(out); },
             *this, out);
  }

  auto empty() {
    return te::call<int>(
        [](auto &self) -> decltype(self.empty()) { return self.empty(); },
        *this);
  }
};

struct Empty {
  void draw(std::ostream &out) const { out << "Empty"; }
  int empty() { return 1; }
};

struct CircleEmpty : Circle {
  int empty() { return 2; }
};

struct SquareEmpty : Square {
  int empty() { return 3; }
};

template <class TDrawable>
requires te::conceptify<TDrawable, DrawableConcept> void draw(
    TDrawable const &drawable, std::ostream &out) {
  drawable.draw(out);
}

test should_support_erasing_using_concepts = [] {
  {
    std::stringstream str{};
    te::var<DrawableConcept> drawable = SquareEmpty{};
    drawable.draw(str);
    expect("Square" == str.str());
    expect(3 == drawable.empty());
  }

  {
    std::stringstream str{};
    auto drawable = SquareEmpty{};
    draw(drawable, str);
    expect("Square" == str.str());
    expect(3 == drawable.empty());
  }

  {
    std::stringstream str{};
    auto drawable = CircleEmpty{};
    draw(drawable, str);
    expect("Circle" == str.str());
    expect(2 == drawable.empty());
  }

  {
    std::stringstream str{};
    auto drawable = Empty{};
    draw(drawable, str);
    expect("Empty" == str.str());
    expect(1 == drawable.empty());
  }
};

struct DrawableConceptMacro {
  auto draw(std::ostream &out) const -> REQUIRES(void, draw, out);
  auto empty() -> REQUIRES(int, empty);
};

test should_support_erasing_using_concepts_macro = [] {
  {
    std::stringstream str{};
    te::var<DrawableConceptMacro> drawable = SquareEmpty{};
    drawable.draw(str);
    expect("Square" == str.str());
    expect(3 == drawable.empty());
  }

  {
    std::stringstream str{};
    auto drawable = Empty{};
    draw(drawable, str);
    expect("Empty" == str.str());
    expect(1 == drawable.empty());
  }
};
#endif
