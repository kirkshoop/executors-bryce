#ifndef STD_EXPERIMENTAL_BITS_REQUIRE_H
#define STD_EXPERIMENTAL_BITS_REQUIRE_H

#include <experimental/bits/has_require_member.h>
#include <experimental/bits/is_twoway_executor.h>
#include <experimental/bits/is_oneway_executor.h>
#include <experimental/bits/is_bulk_oneway_executor.h>
#include <experimental/bits/is_bulk_twoway_executor.h>
#include <utility>

namespace std {
namespace experimental {
inline namespace executors_v1 {
namespace execution {

namespace require_impl {

template<class Executor, class Property>
constexpr auto require(Executor&& ex, Property&& p)
  -> decltype(std::forward<Executor>(ex).require(std::forward<Property>(p)))
{
  return std::forward<Executor>(ex).require(std::forward<Property>(p));
}

struct require_fn
{
  template<class Executor, class Property>
  constexpr auto operator()(Executor&& ex, Property&& p) const
    noexcept(noexcept(require(std::forward<Executor>(ex), std::forward<Property>(p))))
    -> typename std::enable_if<std::decay<Property>::type::is_requirable,
      decltype(require(std::forward<Executor>(ex), std::forward<Property>(p)))>::type
  {
    static_assert(std::decay<Property>::type::template is_supportable<
      decltype(require(std::forward<Executor>(ex), std::forward<Property>(p)))>,
        "requested property is not supportable by resulting executor type");
    return require(std::forward<Executor>(ex), std::forward<Property>(p));
  }

  template<class Executor, class Property0, class Property1, class... PropertyN>
  constexpr auto operator()(Executor&& ex, Property0&& p0, Property1&& p1, PropertyN&&... pn) const
    noexcept(noexcept(std::declval<require_fn>()(std::declval<require_fn>()(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...)))
    -> decltype(std::declval<require_fn>()(std::declval<require_fn>()(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...))
  {
    return (*this)((*this)(std::forward<Executor>(ex), std::forward<Property0>(p0)), std::forward<Property1>(p1), std::forward<PropertyN>(pn)...);
  }
};

template<class T = require_fn> constexpr T customization_point{};

} // namespace require_impl
} // namespace execution
} // inline namespace executors_v1
} // namespace experimental
} // namespace std

#endif // STD_EXPERIMENTAL_BITS_REQUIRE_H
