#include "../performance/timer.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <execution>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

int solution1() {
    std::array<std::vector<int>, 2> data;
    data[0].reserve(200);
    data[1].reserve(200);
    std::string text;
    for (int i = 0; std::cin >> text; ++i) {
        int number = std::stoi(text);
        data[i % 2].emplace_back(number);
    }
    Timer start;

    std::for_each(data.begin(), data.end(), [](auto &vec) -> void {
        std::sort(vec.begin(), vec.end(), std::less<int>{});
    });

    int res = 0;
    for (auto iter1 = data[0].cbegin(), iter2 = data[1].cbegin();
         iter1 != data[0].cend(); ++iter1, ++iter2) {
        res += std::abs(*iter1 - *iter2);
    }
    return res;
}

int solution2() {
    std::unordered_map<int, int> left_freq, right_freq;
    std::string left, right;
    for (int i = 0; std::cin >> left >> right; ++i) {
        left_freq[std::stoi(left)]++;
        right_freq[std::stoi(right)]++;
    }
    Timer start;
    return std::transform_reduce(
        std::execution::par_unseq, left_freq.cbegin(), left_freq.cend(), 0,
        std::plus{}, [&](const auto &pair) -> int {
            auto res = pair.first * pair.second;
            if (res == 0) {
                return 0;
            }
            return res * ([&]() {
                       auto it = right_freq.find(pair.first);
                       return it != right_freq.end() ? it->second : 0;
                   })();
        });
}

int main(int argc, char **argv) {
    printf("%d\n",
           ((std::stoi(argv[argc - 1]) - 1) ? solution2() : solution1()));
}
