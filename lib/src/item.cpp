#include <poc/item.hpp>

#include <cassert>

namespace poc {

auto item::is_magic_blob(std::string_view blob) noexcept -> bool
{
    return blob.starts_with("Item Class:") && blob.contains("Rarity: Magic");
}

auto item::parse_magic_name(std::string_view blob) noexcept -> std::string_view
{
    assert(is_magic_blob(blob) && "blob must be a magic item");

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
