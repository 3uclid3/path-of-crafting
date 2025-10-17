#include <doctest/doctest.h>

#include <poc.core/container/static_vector.hpp>

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include <poc.core/assert.hpp>

namespace poc { namespace {

struct non_trivial
{
    struct method_calls
    {
        std::size_t ctored_default{0};

        std::vector<int> ctored;
        std::vector<int> ctored_copied;
        std::vector<int> ctored_moved;
        std::vector<int> copied;
        std::vector<int> moved;
        std::vector<int> dtored;

        void reset() noexcept
        {
            ctored_default = 0;
            ctored.clear();
            ctored_copied.clear();
            ctored_moved.clear();
            copied.clear();
            moved.clear();
            dtored.clear();
        }
    };

    non_trivial() noexcept
    {
        POC_ASSERT(calls);
        calls->ctored_default++;
    }

    non_trivial(const non_trivial& other)
        : value(other.value)
    {
        POC_ASSERT(calls);
        calls->ctored_copied.push_back(value);
    }

    non_trivial(non_trivial&& other)
        : value(other.value)
    {
        POC_ASSERT(calls);
        calls->ctored_moved.push_back(value);
    }

    non_trivial(int value)
        : value(value)
    {
        POC_ASSERT(calls);
        calls->ctored.push_back(value);
    }

    auto operator=(const non_trivial& other) -> non_trivial&
    {
        if (this != &other)
        {
            value = other.value;

            POC_ASSERT(calls);
            calls->copied.push_back(value);
        }

        return *this;
    }

    auto operator=(non_trivial&& other) -> non_trivial&
    {
        if (this != &other)
        {
            value = other.value;

            POC_ASSERT(calls);
            calls->moved.push_back(value);
        }

        return *this;
    }

    ~non_trivial()
    {
        POC_ASSERT(calls);
        calls->dtored.push_back(value);
    }

    int value{std::numeric_limits<int>::max()};

    static inline method_calls* calls{nullptr};
};

struct trivial_fixture
{
    using static_vector_type = static_vector<int, 4>;

    static_vector_type vector;
};

struct non_trivial_fixture
{
    using static_vector_type = static_vector<non_trivial, 4>;

    non_trivial_fixture()
    {
        non_trivial::calls = &calls;
    }

    ~non_trivial_fixture()
    {
        vector.clear();

        non_trivial::calls = nullptr;
    }

    static_vector_type vector;

    non_trivial::method_calls calls;
};

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: default constructor")
{
    CHECK_EQ(vector.size(), 0);
    CHECK(vector.empty());
    CHECK_FALSE(vector.full());

    CHECK_EQ(calls.ctored_default, 0);
    CHECK(calls.ctored.empty());
    CHECK(calls.ctored_copied.empty());
    CHECK(calls.ctored_moved.empty());
    CHECK(calls.copied.empty());
    CHECK(calls.moved.empty());
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: push_back move")
{
    non_trivial v0{0};
    non_trivial v1{1};
    non_trivial v2{2};
    non_trivial v3{3};
    non_trivial v4{4};

    calls.reset();

    CHECK(vector.push_back(std::move(v0)));
    CHECK(vector.push_back(std::move(v1)));
    CHECK(vector.push_back(std::move(v2)));
    CHECK(vector.push_back(std::move(v3)));
    CHECK_FALSE(vector.push_back(std::move(v4)));

    CHECK_EQ(vector.size(), 4);
    CHECK(vector.full());

    CHECK_EQ(calls.ctored_moved, std::vector<int>{0, 1, 2, 3});

    CHECK(calls.ctored.empty());
    CHECK(calls.ctored_copied.empty());
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: push_back copy")
{
    const non_trivial v0{0};
    const non_trivial v1{1};
    const non_trivial v2{2};
    const non_trivial v3{3};
    const non_trivial v4{4};

    calls.reset();

    CHECK(vector.push_back(v0));
    CHECK(vector.push_back(v1));
    CHECK(vector.push_back(v2));
    CHECK(vector.push_back(v3));
    CHECK_FALSE(vector.push_back(v4));

    CHECK_EQ(vector.size(), 4);
    CHECK(vector.full());

    CHECK_EQ(calls.ctored_copied, std::vector<int>{0, 1, 2, 3});

    CHECK(calls.ctored.empty());
    CHECK(calls.ctored_moved.empty());
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: emplace_back")
{
    CHECK(vector.emplace_back(0));
    CHECK(vector.emplace_back(1));
    CHECK(vector.emplace_back(2));
    CHECK(vector.emplace_back(3));
    CHECK_FALSE(vector.emplace_back(4));

    CHECK_EQ(vector.size(), 4);
    CHECK(vector.full());

    CHECK_EQ(calls.ctored, std::vector<int>{0, 1, 2, 3});
    CHECK(calls.ctored_copied.empty());
    CHECK(calls.ctored_moved.empty());
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: pop_back destroys last element")
{
    vector.emplace_back(0);
    vector.emplace_back(1);
    vector.emplace_back(2);

    calls.reset();
    vector.pop_back();
    CHECK_EQ(vector.size(), 2);
    CHECK_EQ(calls.dtored, std::vector<int>{2});

    calls.reset();
    vector.pop_back();
    CHECK_EQ(vector.size(), 1);
    CHECK_EQ(calls.dtored, std::vector<int>{1});

    calls.reset();
    vector.pop_back();
    CHECK_EQ(vector.size(), 0);
    CHECK_EQ(calls.dtored, std::vector<int>{0});

    calls.reset();
    vector.pop_back();
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: clear destroys all elements")
{
    vector.emplace_back(0);
    vector.emplace_back(1);
    vector.emplace_back(2);

    calls.reset();

    vector.clear();

    CHECK(vector.empty());
    CHECK_EQ(calls.dtored, std::vector<int>{0, 1, 2});

    calls.reset();
    vector.clear();
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: copy constructor copies elements")
{
    static_vector<non_trivial, 4> source;
    CHECK(source.emplace_back(0));
    CHECK(source.emplace_back(1));
    CHECK(source.emplace_back(2));

    calls.reset();

    static_vector<non_trivial, 4> copy(source);

    CHECK_EQ(copy.size(), 3);
    CHECK_EQ(source.size(), 3);
    CHECK_EQ(copy[0].value, 0);
    CHECK_EQ(copy[1].value, 1);
    CHECK_EQ(copy[2].value, 2);

    CHECK_EQ(calls.ctored_copied, std::vector<int>{0, 1, 2});
    CHECK(calls.ctored_moved.empty());
    CHECK(calls.dtored.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: move constructor moves and clears source")
{
    static_vector<non_trivial, 4> source;
    CHECK(source.emplace_back(0));
    CHECK(source.emplace_back(1));
    CHECK(source.emplace_back(2));

    calls.reset();

    static_vector<non_trivial, 4> moved(std::move(source));

    CHECK_EQ(moved.size(), 3);
    CHECK(source.empty());
    CHECK_EQ(moved[0].value, 0);
    CHECK_EQ(moved[1].value, 1);
    CHECK_EQ(moved[2].value, 2);

    CHECK_EQ(calls.ctored_moved, std::vector<int>{0, 1, 2});
    CHECK_EQ(calls.dtored, std::vector<int>{0, 1, 2});
    CHECK(calls.ctored_copied.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: copy assignment clears target and copies source")
{
    static_vector<non_trivial, 4> source;
    CHECK(source.emplace_back(0));
    CHECK(source.emplace_back(1));
    CHECK(source.emplace_back(2));

    static_vector<non_trivial, 4> target;
    CHECK(target.emplace_back(42));
    CHECK(target.emplace_back(43));

    calls.reset();

    target = source;

    CHECK_EQ(target.size(), 3);
    CHECK_EQ(source.size(), 3);
    CHECK_EQ(target[0].value, 0);
    CHECK_EQ(target[1].value, 1);
    CHECK_EQ(target[2].value, 2);

    CHECK_EQ(calls.dtored, std::vector<int>{42, 43});
    CHECK_EQ(calls.ctored_copied, std::vector<int>{0, 1, 2});
    CHECK(calls.ctored_moved.empty());
    CHECK(calls.copied.empty());
    CHECK(calls.moved.empty());
}

TEST_CASE_FIXTURE(non_trivial_fixture, "static_vector<non_trivial>: move assignment clears target and moves source")
{
    static_vector<non_trivial, 4> source;
    CHECK(source.emplace_back(0));
    CHECK(source.emplace_back(1));
    CHECK(source.emplace_back(2));

    static_vector<non_trivial, 4> target;
    CHECK(target.emplace_back(42));
    CHECK(target.emplace_back(43));

    calls.reset();

    target = std::move(source);

    CHECK_EQ(target.size(), 3);
    CHECK(source.empty());
    CHECK_EQ(target[0].value, 0);
    CHECK_EQ(target[1].value, 1);
    CHECK_EQ(target[2].value, 2);

    CHECK_EQ(calls.dtored, std::vector<int>{42, 43, 0, 1, 2});
    CHECK_EQ(calls.ctored_moved, std::vector<int>{0, 1, 2});
    CHECK(calls.ctored_copied.empty());
    CHECK(calls.copied.empty());
    CHECK(calls.moved.empty());
}

}} // namespace poc
