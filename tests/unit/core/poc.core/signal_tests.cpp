#include <doctest/doctest.h>

#include <poc.core/signal.hpp>

#include <vector>

namespace poc { namespace {

struct object
{
    auto set_value(int v) -> void
    {
        value = v;
    }

    int value{0};
};

TEST_CASE("basic_inplace_signal: empty")
{
    inplace_signal<1, int> signal;

    CHECK(signal.empty());
    CHECK_EQ(signal.size(), 0);
}

TEST_CASE("basic_signal_connector: connect using object pointer")
{
    inplace_signal<1, int> signal;
    object obj;

    auto connection = signal.connector().connect(&obj, &object::set_value);

    signal.emit(13);
    CHECK_EQ(obj.value, 13);
}

TEST_CASE("basic_signal_connector: connect using object reference")
{
    inplace_signal<1, int> signal;
    object obj;

    auto connection = signal.connector().connect(obj, &object::set_value);

    signal.emit(27);
    CHECK_EQ(obj.value, 27);
}

TEST_CASE("basic_inplace_signal: zero inplace capacity migrates to heap immediately")
{
    inplace_signal<0, int> signal;

    CHECK(signal.empty());
    CHECK_EQ(signal.capacity(), 0);
    CHECK_FALSE(signal.is_heap());

    int tally = 0;
    auto connection = signal.connector().connect([&](int value) { tally += value; });

    CHECK(signal.is_heap());
    CHECK_GE(signal.capacity(), 1);
    CHECK_EQ(signal.size(), 1);

    signal.emit(5);
    CHECK_EQ(tally, 5);

    connection.disconnect();
    CHECK(signal.empty());
    CHECK(signal.is_heap());

    signal.reset();
    CHECK(signal.empty());
    CHECK_FALSE(signal.is_heap());
    CHECK_EQ(signal.capacity(), 0);
}

TEST_CASE("basic_inplace_signal: emit forwards arguments to slots")
{
    inplace_signal<2, int> signal;

    int sum = 0;
    auto connection = signal.connector().connect([&](int value) { sum += value; });

    signal.emit(5);
    CHECK_EQ(sum, 5);
    CHECK_EQ(signal.size(), 1);

    connection.disconnect();
    signal.emit(7);
    CHECK_EQ(sum, 5);
}

TEST_CASE("basic_inplace_signal: connection destruction auto-disconnects")
{
    inplace_signal<1> signal;
    bool invoked = false;

    {
        auto connection = signal.connector().connect([&]() { invoked = true; });
        CHECK(connection.is_connected());
        CHECK_EQ(signal.size(), 1);
    }

    CHECK_EQ(signal.size(), 0);
    signal.emit();
    CHECK_FALSE(invoked);
}

TEST_CASE("basic_inplace_signal: scoped_block suppresses emission")
{
    inplace_signal<1, int> signal;
    int observed = 0;
    auto connection = signal.connector().connect([&](int value) { observed = value; });

    {
        auto guard = signal.block();
        CHECK(signal.blocked());
        signal.emit(42);
        CHECK_EQ(observed, 0);
    }

    CHECK_FALSE(signal.blocked());
    signal.emit(7);
    CHECK_EQ(observed, 7);
}

TEST_CASE("basic_inplace_signal: migrates to heap when inline capacity exceeded")
{
    inplace_signal<1, int> signal;
    std::vector<int> hits;

    auto make_slot = [&](int id) {
        return signal.connector().connect([&, id](int value) { hits.push_back(id * 10 + value); });
    };

    auto c0 = make_slot(0);
    auto c1 = make_slot(1);
    auto c2 = make_slot(2);

    CHECK(signal.is_heap());
    CHECK_EQ(signal.size(), 3);
    CHECK_GE(signal.capacity(), 3);

    signal.emit(5);
    CHECK_EQ(hits, std::vector<int>{5, 15, 25});

    signal.reset();
    CHECK(signal.empty());
    CHECK_FALSE(signal.is_heap());
}

TEST_CASE("basic_inplace_signal: connection release detaches handle but leaves slot active")
{
    inplace_signal<1, int> signal;
    int sum = 0;

    auto connection = signal.connector().connect([&](int value) { sum += value; });
    CHECK(connection.is_connected());

    connection.release();
    CHECK_FALSE(connection.is_connected());

    signal.emit(3);
    CHECK_EQ(sum, 3);
    CHECK_EQ(signal.size(), 1);

    signal.disconnect_all();
    CHECK(signal.empty());
}

TEST_CASE("basic_inplace_signal: slot can disconnect itself during emit")
{
    inplace_signal<2> signal;
    std::vector<int> hits;

    inplace_signal<2>::connection_type self;
    self = signal.connector().connect([&]() {
        hits.push_back(1);
        CHECK(self.is_connected());
        self.disconnect();
        CHECK_FALSE(self.is_connected());
    });

    auto other = signal.connector().connect([&]() { hits.push_back(2); });

    signal.emit();

    CHECK_EQ(hits, std::vector<int>{1, 2});
    CHECK_FALSE(self.is_connected());
    CHECK(other.is_connected());
    CHECK_EQ(signal.size(), 1);

    other.disconnect();
}

TEST_CASE("basic_inplace_signal: slot can disconnect another slot during emit")
{
    inplace_signal<2> signal;
    std::vector<int> hits;

    inplace_signal<2>::connection_type target;
    auto dropper = signal.connector().connect([&]() {
        hits.push_back(1);
        target.disconnect();
    });

    target = signal.connector().connect([&]() { hits.push_back(2); });

    signal.emit();

    CHECK_EQ(hits, std::vector<int>{1});
    CHECK(dropper.is_connected());
    CHECK_FALSE(target.is_connected());
    CHECK_EQ(signal.size(), 1);

    dropper.disconnect();
}

TEST_CASE("basic_inplace_signal: disconnect_all can be invoked during emit")
{
    inplace_signal<3, int> signal;
    std::vector<int> hits;

    auto first = signal.connector().connect([&](int value) {
        hits.push_back(10 + value);
        signal.disconnect_all();
    });

    auto second = signal.connector().connect([&](int value) { hits.push_back(20 + value); });
    auto third = signal.connector().connect([&](int value) { hits.push_back(30 + value); });

    signal.emit(5);

    CHECK_EQ(hits, std::vector<int>{15});
    CHECK(signal.empty());
    CHECK_FALSE(first.is_connected());
    CHECK_FALSE(second.is_connected());
    CHECK_FALSE(third.is_connected());
}

TEST_CASE("basic_inplace_signal: disconnect_all preserves heap allocation until reset")
{
    inplace_signal<1> signal;

    auto c0 = signal.connector().connect([]() {});
    auto c1 = signal.connector().connect([]() {});

    CHECK(signal.is_heap());
    CHECK_EQ(signal.size(), 2);

    signal.disconnect_all();
    CHECK(signal.empty());
    CHECK(signal.is_heap());

    signal.reset();
    CHECK_FALSE(signal.is_heap());
}

TEST_CASE("basic_inplace_signal: reserve preallocates heap capacity")
{
    inplace_signal<1, int> signal;

    signal.reserve(4);
    CHECK(signal.is_heap());
    CHECK_GE(signal.capacity(), 4);

    int observed = 0;
    auto connection = signal.connector().connect([&](int value) { observed += value; });

    signal.emit(5);
    CHECK_EQ(observed, 5);
    connection.disconnect();
}

TEST_CASE("basic_inplace_signal: nested blocks stack correctly")
{
    inplace_signal<1, int> signal;
    int tally = 0;
    auto connection = signal.connector().connect([&](int value) { tally += value; });

    {
        auto outer = signal.block();
        CHECK(signal.blocked());

        {
            auto inner = signal.block();
            CHECK(signal.blocked());
            signal.emit(10);
            CHECK_EQ(tally, 0);
        }

        CHECK(signal.blocked());
    }

    CHECK_FALSE(signal.blocked());
    signal.emit(7);
    CHECK_EQ(tally, 7);

    connection.disconnect();
}

TEST_CASE("basic_inplace_signal: connection move transfers ownership")
{
    inplace_signal<1> signal;
    int invokes = 0;

    auto original = signal.connector().connect([&]() { ++invokes; });
    CHECK(original.is_connected());

    auto moved = std::move(original);
    CHECK_FALSE(original.is_connected());
    CHECK(moved.is_connected());

    signal.emit();
    CHECK_EQ(invokes, 1);

    moved.disconnect();
    CHECK_EQ(signal.size(), 0);
}

TEST_CASE("basic_inplace_signal: move constructor transfers empty state")
{
    inplace_signal<2, int> source;
    int moved_sum = 0;
    int source_sum = 0;

    source.reserve(4);
    CHECK(source.is_heap());
    CHECK(source.empty());

    auto moved = std::move(source);

    CHECK(moved.is_heap());
    CHECK_GE(moved.capacity(), 4);
    CHECK(moved.empty());

    CHECK_FALSE(source.is_heap());
    CHECK_EQ(source.capacity(), inplace_signal<2, int>::inplace_size);
    CHECK(source.empty());

    auto moved_connection = moved.connector().connect([&](int value) { moved_sum += value; });
    moved.emit(3);
    CHECK_EQ(moved_sum, 3);
    moved_connection.disconnect();
    CHECK(moved.empty());

    auto source_connection = source.connector().connect([&](int value) { source_sum += value; });
    source.emit(5);
    CHECK_EQ(source_sum, 5);
    source_connection.disconnect();
    CHECK(source.empty());
}

TEST_CASE("basic_inplace_signal: move assignment transfers empty state")
{
    inplace_signal<2, int> target;
    int target_sum = 0;
    int source_sum = 0;

    target.reserve(3);
    CHECK(target.is_heap());
    CHECK(target.empty());

    inplace_signal<2, int> source;
    source.reserve(5);
    CHECK(source.is_heap());
    CHECK(source.empty());

    target = std::move(source);

    CHECK(target.is_heap());
    CHECK_GE(target.capacity(), 5);
    CHECK(target.empty());

    CHECK_FALSE(source.is_heap());
    CHECK_EQ(source.capacity(), inplace_signal<2, int>::inplace_size);
    CHECK(source.empty());

    auto connection = target.connector().connect([&](int value) { target_sum += value; });
    target.emit(6);
    CHECK_EQ(target_sum, 6);
    target.emit(4);
    CHECK_EQ(target_sum, 10);
    connection.disconnect();
    CHECK(target.empty());

    source.reserve(3);
    CHECK(source.is_heap());
    auto source_connection = source.connector().connect([&](int value) { source_sum += value; });
    source.emit(3);
    CHECK_EQ(source_sum, 3);
    source_connection.disconnect();
    CHECK(source.empty());
}

}} // namespace poc
