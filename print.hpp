#pragma once

#include <experimental/type_traits>
#include <sstream>
#include <type_traits>


namespace print
{

template <typename T>
using has_cbegin = decltype(&T::cbegin);

template <typename T>
using has_cend = decltype(&T::cend);

template <typename T>
using has_key_type = typename T::key_type;

template <typename T>
using has_mapped_type = typename T::mapped_type;

template <typename T>
using has_to_string_method = decltype(&T::to_string);


template <typename T>
constexpr bool is_map_v =
    std::experimental::is_detected_v<has_key_type, T> &&
    std::experimental::is_detected_v<has_mapped_type, T>;

template <typename T>
constexpr bool is_list_container_v =
    std::experimental::is_detected_v<has_cbegin, T> &&
    std::experimental::is_detected_v<has_cend, T> &&
    !is_map_v<T>;


template <typename T>
std::enable_if_t<is_list_container_v<T>,
std::string> to_string(const T& list)
{
    std::stringstream result;
    result << "[";
    auto cb = cbegin(list);
    auto ce = cend(list);

    if (cb != ce)
    {
        result << *cb;
        ++cb;
    }
    for (; cb != ce; ++cb)
    {
        result << ", " << *cb;
    }
    result << "]";
    return result.str();
}

template<typename T>
std::enable_if_t<is_map_v<T>,
std::string> to_string(const T& map)
{
    using namespace std;
    std::stringstream result;
    result << "{";
    auto cb = cbegin(map);
    auto ce = cend(map);

    if (cb != ce)
    {
        result << "{" << cb->first << ": " << cb->second << "}";
        ++cb;
    }
    for (; cb != ce; ++cb)
    {
        result << ", {" << cb->first << ": " << cb->second << "}";        
    }
    result << "}";
    return result.str();
}

template<typename T>
std::enable_if_t<
    std::experimental::is_detected_v<has_to_string_method, T> &&
    std::experimental::is_pointer_v<T>,
std::string> to_string(const T& value)
{
    return value->to_string();
}

template<typename T>
std::enable_if_t<
    std::experimental::is_detected_v<has_to_string_method, T> &&
    !std::experimental::is_pointer_v<T>,
std::string> to_string(const T& value)
{
    return value.to_string();
}

template <typename T>
std::enable_if_t<std::experimental::is_same_v<T, std::string>,
std::string> str(const T& value)
{
    return value;
}

namespace tostring
{
namespace details
{
namespace impl
{
using namespace std;

template <typename T>
using has_to_string = decltype(to_string(T()));
}  //  namespace impl
}  //  namespace details
}  //  namespace tostring


template <typename T>
std::enable_if_t<
    !std::experimental::is_same_v<T, std::string> &&
    std::experimental::is_detected_v<tostring::details::impl::has_to_string, T>,
std::string> str(const T& value)
{
    using namespace std;
    return to_string(value);
}

template <typename T>
std::enable_if_t<
    !std::experimental::is_same_v<T, std::string> &&
    !std::experimental::is_detected_v<tostring::details::impl::has_to_string, T>,
std::string> str(const T& value)
{
    return "Class has no const 'to_string' method defined";
}

} // namespace print
