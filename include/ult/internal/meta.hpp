#pragma once

namespace ult::internal {

template <class T, class U>
struct IsSame {
  static constexpr bool value = false;
};

template <class T>
struct IsSame<T, T> {
  static constexpr bool value = true;
};

template <class T>
T a_value() noexcept;

template <class T>
void test_convert(T);

template <class From, class To, class = void>
struct IsConvertibleToImpl {
  static constexpr bool value = false;
};

template <class From, class To>
struct IsConvertibleToImpl<From, To, decltype(test_convert<To>(a_value<From>()))> {
  static constexpr bool value = true;
};

template <class From, class To>
struct IsConvertibleTo {
  static constexpr bool value = IsConvertibleToImpl<From, To>::value;
};

}  // namespace ult::internal
