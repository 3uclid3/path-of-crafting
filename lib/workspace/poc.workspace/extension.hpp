#pragma once

#include <concepts>

#include <poc.core/types.hpp>

namespace poc::workspace::extension {

// The host will fail to initialize the extension if it returns anything other than success.
enum class init_result
{
    success,

    failure,
    dependency_missing
};

class init_context
{
public:
    template<typename T>
    requires std::derived_from<T, extension>
    auto find_extension() -> extension*;
    auto find_extension(string_view id) -> extension*;
};

class update_context
{
public:
};

class interface
{
public:
    static constexpr string_view id = "extension";

    virtual ~interface() = default;

    virtual auto id() const -> string_view = 0;

    virtual auto init(init_context& context) -> init_result = 0;
    virtual auto update(update_context& context) -> void = 0;
};

} // namespace poc::workspace::extension
