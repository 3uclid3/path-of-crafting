#include <doctest/doctest.h>

#include <poc/item_line_index.hpp>

namespace poc {
namespace {

static const string raw_divider = "--------";
static const string raw_key_value = "key: value";
static const string raw_braced = "{ braced content }";
static const string raw_text = "just some text";

static auto check_divider(const item_line_index::line& line, std::size_t offset, bool has_lf, bool has_cr) -> void
{
    CHECK_EQ(line.has_lf(), has_lf);
    CHECK_EQ(line.has_cr(), has_cr);

    CHECK_EQ(line.kind(), item_line_index::line_kind::divider);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), raw_divider);
}

static auto check_key_value(const item_line_index::line& line, std::size_t offset, bool has_lf, bool has_cr) -> void
{
    CHECK_EQ(line.has_lf(), has_lf);
    CHECK_EQ(line.has_cr(), has_cr);

    CHECK_EQ(line.kind(), item_line_index::line_kind::key_value);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), raw_key_value);
}

static auto check_braced(const item_line_index::line& line, std::size_t offset, bool has_lf, bool has_cr) -> void
{
    CHECK_EQ(line.has_lf(), has_lf);
    CHECK_EQ(line.has_cr(), has_cr);

    CHECK_EQ(line.kind(), item_line_index::line_kind::braced);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), raw_braced);
}

static auto check_text(const item_line_index::line& line, std::size_t offset, bool has_lf, bool has_cr) -> void
{
    CHECK_EQ(line.has_lf(), has_lf);
    CHECK_EQ(line.has_cr(), has_cr);

    CHECK_EQ(line.kind(), item_line_index::line_kind::text);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), raw_text);
};

static auto check_empty(const item_line_index::line& line, std::size_t offset, bool has_lf, bool has_cr) -> void
{
    CHECK_EQ(line.has_lf(), has_lf);
    CHECK_EQ(line.has_cr(), has_cr);

    CHECK_EQ(line.kind(), item_line_index::line_kind::empty);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), string_view{});
};

static auto check_end(const item_line_index::line& line, std::size_t offset) -> void
{
    CHECK_FALSE(line.has_lf());
    CHECK_FALSE(line.has_cr());

    CHECK_EQ(line.kind(), item_line_index::line_kind::end);
    CHECK_EQ(line.offset(), offset);
    CHECK_EQ(line.raw(), string_view{});
}

static constexpr bool has_cr = true;
static constexpr bool has_lf = true;
static constexpr bool no_cr = false;
static constexpr bool no_lf = false;

TEST_CASE("item_line_index: divider line")
{
    SUBCASE("no newline")
    {
        item_line_index src(raw_divider);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_divider);

        check_divider(src[0], 0, no_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lf")
    {
        string raw_lf = raw_divider + "\n";
        item_line_index src(raw_lf);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lf);

        check_divider(src[0], 0, has_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lfcr")
    {
        string raw_lfcr = raw_divider + "\r\n";
        item_line_index src(raw_lfcr);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lfcr);

        check_divider(src[0], 0, has_lf, has_cr);
        check_end(src[1], src.raw().size());
    }
}

TEST_CASE("item_line_index: key_value line")
{
    SUBCASE("no newline")
    {
        item_line_index src(raw_key_value);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_key_value);

        check_key_value(src[0], 0, no_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lf")
    {
        string raw_lf = raw_key_value + "\n";
        item_line_index src(raw_lf);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lf);

        check_key_value(src[0], 0, has_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lfcr")
    {
        string raw_lfcr = raw_key_value + "\r\n";
        item_line_index src(raw_lfcr);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lfcr);

        check_key_value(src[0], 0, has_lf, has_cr);
        check_end(src[1], src.raw().size());
    }
}

TEST_CASE("item_line_index: braced line")
{
    SUBCASE("no newline")
    {
        item_line_index src(raw_braced);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_braced);

        check_braced(src[0], 0, no_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lf")
    {
        string raw_lf = raw_braced + "\n";
        item_line_index src(raw_lf);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lf);

        check_braced(src[0], 0, has_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lfcr")
    {
        string raw_lfcr = raw_braced + "\r\n";
        item_line_index src(raw_lfcr);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lfcr);

        check_braced(src[0], 0, has_lf, has_cr);
        check_end(src[1], src.raw().size());
    }
}

TEST_CASE("item_line_index: text line")
{
    SUBCASE("no newline")
    {
        item_line_index src(raw_text);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_text);

        check_text(src[0], 0, no_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lf")
    {
        string raw_lf = raw_text + "\n";
        item_line_index src(raw_lf);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lf);

        check_text(src[0], 0, has_lf, no_cr);
        check_end(src[1], src.raw().size());
    }

    SUBCASE("with lfcr")
    {
        string raw_lfcr = raw_text + "\r\n";
        item_line_index src(raw_lfcr);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw_lfcr);

        check_text(src[0], 0, has_lf, has_cr);
        check_end(src[1], src.raw().size());
    }
}

TEST_CASE("item_line_index: multiple lines")
{
    string raw = raw_key_value + "\n" + raw_key_value + "\r\n" + raw_text + "\n" + raw_braced + "\n";

    item_line_index src(raw);

    REQUIRE_EQ(src.size(), 5);
    CHECK_EQ(src.raw(), raw);

    std::size_t offset = 0;

    check_key_value(src[0], offset, has_lf, no_cr);
    offset += raw_key_value.size() + 1;

    check_key_value(src[1], offset, has_lf, has_cr);
    offset += raw_key_value.size() + 2;

    check_text(src[2], offset, has_lf, no_cr);
    offset += raw_text.size() + 1;

    check_braced(src[3], offset, has_lf, no_cr);

    check_end(src[4], src.raw().size());
}

TEST_CASE("item_line_index::iterator")
{
    string raw = raw_key_value + "\n" + raw_key_value + "\r\n" + raw_text + "\n" + raw_braced + "\n";

    item_line_index src(raw);

    REQUIRE_EQ(src.size(), 5);
    CHECK_EQ(src.raw(), raw);

    std::size_t offset = 0;

    auto it = src.begin();
    auto end = src.end();

    REQUIRE_NE(it, end);
    check_key_value(*it, offset, has_lf, no_cr);
    offset += raw_key_value.size() + 1;
    ++it;

    REQUIRE_NE(it, end);
    check_key_value(*it, offset, has_lf, has_cr);
    offset += raw_key_value.size() + 2;
    it++;

    REQUIRE_NE(it, end);
    check_text(*it, offset, has_lf, no_cr);
    offset += raw_text.size() + 1;
    ++it;

    REQUIRE_NE(it, end);
    check_braced(*it, offset, has_lf, no_cr);
    ++it;

    REQUIRE_NE(it, end);
    check_end(*it, src.raw().size());
    ++it;

    CHECK(it == end);
}

TEST_CASE("item_line_index: empty line")
{
    SUBCASE("with lf")
    {
        string_view raw = "\n";
        item_line_index src(raw);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw);

        check_empty(src[0], 0, has_lf, no_cr);
        check_end(src[1], raw.size());
    }

    SUBCASE("with lfcr")
    {
        string_view raw = "\r\n";
        item_line_index src(raw);

        REQUIRE_EQ(src.size(), 2);
        CHECK_EQ(src.raw(), raw);

        check_empty(src[0], 0, has_lf, has_cr);
        check_end(src[1], raw.size());
    }
}

TEST_CASE("item_line_index: empty")
{
    string_view raw = "";
    item_line_index src(raw);

    REQUIRE_EQ(src.size(), 1);
    CHECK_EQ(src.raw(), raw);

    check_end(src[0], raw.size());
}

}} // namespace poc
