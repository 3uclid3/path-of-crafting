#include <poc.core/types.hpp>

namespace poc {

template class basic_string_arena<char, default_string_arena_page_size>;
template class basic_string_slice<char>;

} // namespace poc
