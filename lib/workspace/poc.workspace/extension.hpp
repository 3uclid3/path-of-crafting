#pragma once

#include <poc.core/types.hpp>
#include <poc.core/utility/crc32.hpp>
#include <poc.core/utility/tagged.hpp>
#include <poc.workspace/context.hpp>

namespace poc::workspace {

using extension_id = tagged<u32, struct extension_id_tag>;

consteval extension_id make_extension_id(string_view str) { return extension_id{compile_crc32(str)}; }

class extension
{
public:
    using id_type = extension_id;

    // The host will fail to initialize the extension if it returns anything other than success.
    enum class init_result
    {
        success,

        failure,
        dependency_missing
    };

    using init_context = workspace::init_context;
    using update_context = workspace::update_context;

    static constexpr id_type static_id = make_extension_id("extension");

    virtual ~extension() = default;

    virtual auto id() const -> id_type = 0;

    virtual auto init(init_context& context) -> init_result = 0;
    virtual auto update(update_context& context) -> void = 0;
};

} // namespace poc::workspace
