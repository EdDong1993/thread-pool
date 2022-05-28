#include "thread_pool.h"

#include <gtest/gtest.h>

namespace test {

template <typename T>
T min(const T &lhs, const T &rsh) {
    return (lhs < rsh) ? lhs : rsh;
}

TEST(thread_pool, call_min) {
    thread_pool p{8};

    std::vector<std::future<int>> int32_futures;
    std::vector<std::future<uint32_t>> uint32_futures;
    std::vector<std::future<std::string>> string_futures;

    for (size_t i = 0; i < 4; ++i) {
        int32_futures.emplace_back(p.push(test::min<int32_t>, i, i * (-1)));
        uint32_futures.emplace_back(p.push(test::min<uint32_t>, i, i / 2));
        std::string a{"a"};
        std::string b{"b"};
        string_futures.emplace_back(p.push(test::min<std::string>, std::to_string(i), std::to_string(i + 4)));
    }
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(int32_futures.at(i).get(), i * (-1));
        EXPECT_EQ(uint32_futures.at(i).get(), i / 2);
        EXPECT_EQ(string_futures.at(i).get(), std::to_string(i));
    }
}

}  // namespace test