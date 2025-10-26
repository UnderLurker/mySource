#ifndef _HAS_TO_STRING_H
#define _HAS_TO_STRING_H

#include <type_traits>

namespace ULGui::canvas::util {
template<typename T, typename = void>
struct HasToString : std::false_type {};

template<typename T>
struct HasToString<T, std::void_t<decltype(std::declval<T>().toString())>> : std::true_type {};

template<typename T>
inline constexpr bool hasToString_v = HasToString<T>::value;
} // namespace ULGui::canvas::util

#endif // !_HAS_TO_STRING_H
