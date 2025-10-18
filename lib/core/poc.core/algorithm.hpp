#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

namespace poc {

// Counts occurrences of `value` in the range `[begin, end)` until `pred` returns true.
template<typename InputIterator, typename T, typename Pred>
constexpr auto count_until(InputIterator begin, InputIterator end, const T& value, Pred pred) -> typename std::iterator_traits<InputIterator>::difference_type
{
    typename std::iterator_traits<InputIterator>::difference_type count = 0;

    auto internal_pred = [&count, &value, &pred](decltype(*begin) elem) {
        return elem == value && pred(++count);
    };

    [[maybe_unused]] auto it = std::find_if(begin, end, internal_pred);
    return count;
}

namespace ranges {

template<typename Range, typename T, typename Pred>
constexpr auto count_until(Range&& range, const T& value, Pred pred) -> decltype(count_until(std::begin(range), std::end(range), value, pred))
{
    return poc::count_until(std::begin(range), std::end(range), value, pred);
}

} // namespace ranges
} // namespace poc
