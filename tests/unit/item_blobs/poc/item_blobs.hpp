#pragma once

#include <poc/item_blob.hpp>
#include <poc/types.hpp>

namespace poc::item_blobs {

class imported
{
public:
    constexpr imported(const char* v)
        : _value(v)
    {
        while (!_value.empty() && (_value.front() == '\n' || _value.front() == '\r'))
        {
            _value.remove_prefix(1);
        }
    }

    constexpr operator item_blob() const noexcept
    {
        return item_blob(_value);
    }

    constexpr operator string_view() const noexcept
    {
        return _value;
    }

private:
    string_view _value;
};

static inline constexpr imported empty = "";

static inline constexpr imported unknown =
#include "items/unknown.txt"
    ;

static inline constexpr imported rarity_normal =
#include "items/rarity_normal.txt"
    ;

static inline constexpr imported rarity_magic =
#include "items/rarity_magic.txt"
    ;

static inline constexpr imported rarity_rare =
#include "items/rarity_rare.txt"
    ;

} // namespace poc::item_blobs
