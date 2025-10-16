#pragma once

#include <cstdint>
#include <format>

#include <poc/types.hpp>

namespace poc {

enum class modifier_kind : u8
{
    implicit,
    prefix,
    suffix
};

} // namespace poc
