#pragma once

namespace ult::internal {

template <class T, class U>
struct IsSameImpl {
  static constexpr bool value = false;
};

template <class T>
struct IsSameImpl<T, T> {
  static constexpr bool value = true;
};

template <class T, class U>
constexpr bool is_same = IsSameImpl<T, U>::value;

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
constexpr bool is_convertible = IsConvertibleToImpl<From, To>::value;

template<class T>
struct RemoveReferenceImpl {
  using type = T;
};

template<class T>
struct RemoveReferenceImpl<T&> {
  using type = T;
};

template<class T>
struct RemoveReferenceImpl<T&&> {
  using type = T;
};

template <class T>
constexpr typename RemoveReferenceImpl<T>::type&& move(T&& t) noexcept {
  using U = typename RemoveReferenceImpl<T>::type;
  return static_cast<U&&>(t);
}

}  // namespace ult::internal
