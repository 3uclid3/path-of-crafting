#include <poc/item.hpp>

#include <cassert>

namespace poc {

namespace detail {

auto is_blob_with_rarity(std::string_view blob, std::string_view rarity_kv) noexcept -> bool
{
    return blob.starts_with("Item Class:") && blob.contains(rarity_kv);
}

} // namespace detail

auto item::is_blob(std::string_view blob) noexcept -> bool
{
    return detail::is_blob_with_rarity(blob, "Rarity:");
}

auto item::is_magic_blob(std::string_view blob) noexcept -> bool
{
    return detail::is_blob_with_rarity(blob, "Rarity: Magic");
}

auto item::parse_name(std::string_view blob) noexcept -> std::string_view
{
    assert(is_blob(blob) && "blob must be an item");

    const std::size_t rarity_line_index = blob.find("Rarity:");
    const std::size_t name_index = blob.find('\n', rarity_line_index) + 1; // Position after the rarity line

    std::size_t name_end_index = blob.find('\n', name_index + 1); // Find the end of the name
    if (name_end_index == std::string_view::npos)
    {
        name_end_index = blob.size();
    }

    return blob.substr(name_index, name_end_index - name_index);
}

} // namespace poc
