#include "../performance/timer.h"

#include <algorithm>
#include <array>
#include <climits>
#include <cstring>
#include <execution>
#include <numeric>
#include <vector>

std::vector<std::vector<int>> read_from_stdin() {
    std::vector<std::vector<int>> res;
    std::string text;
    int i = 0;
    while (std::getline(std::cin, text, '\n')) {
        res.emplace_back();
        auto &vec = res[i];
        const char *delim = " ";
        char *number = strtok(text.data(), delim);
        while (number != nullptr) {
            vec.emplace_back(atoi(number));
            number = strtok(nullptr, delim);
        }
        i++;
    }
    return res;
}
constexpr std::array<int, 7> increasing_lookup_table{0, 0, 0, 0, 1, 1, 1};
constexpr std::array<int, 7> decreasing_lookup_table{1, 1, 1, 0, 0, 0, 0};
constexpr std::array<std::array<int, 7>, 2> lookup_table{
    increasing_lookup_table, decreasing_lookup_table};

int solution1() {
    auto data = read_from_stdin();
    Timer start;
    return std::count_if(data.begin(), data.end(), [](const auto &vec) -> bool {
        if (vec[1] == vec[0]) {
            return false;
        }
        bool is_decreasing = vec[1] < vec[0];
        for (std::size_t i = 1; i < vec.size(); ++i) {
            int diff = vec[i] - vec[i - 1];
            if ((std::abs(diff) > 3) ||
                lookup_table[is_decreasing][diff + 3] == 0) {
                return false;
            }
        }
        return true;
    });
}

int solution2() {
    auto data = read_from_stdin();
    Timer start;
    return std::count_if(data.begin(), data.end(), [](const auto &vec) -> bool {
        auto diff_is_safe = [&](std::size_t idx1, std::size_t idx2,
                                bool is_decreasing) -> bool {
            int diff = vec[idx2] - vec[idx1];
            return (std::abs(diff) <= 3) &&
                   lookup_table[is_decreasing][diff + 3] != 0;
        };

        auto rest_is_safe = [&](std::size_t i, bool is_decreasing) -> bool {
            while (i < vec.size()) {
                if (!diff_is_safe(i - 1, i, is_decreasing)) {
                    return false;
                }
                i++;
            }
            return true;
        };

        auto can_safely_remove = [&](std::size_t i,
                                     bool is_decreasing = false) -> bool {
            is_decreasing = i >= 2   ? is_decreasing
                            : i == 1 ? vec[2] < vec[0]
                                     : vec[2] < vec[1];

            return i != 0 ? (diff_is_safe(i - 1, i + 1, is_decreasing) &&
                             rest_is_safe(i + 2, is_decreasing))
                          : (diff_is_safe(1, 2, vec[2] < vec[1]) &&
                             rest_is_safe(3, vec[2] < vec[1]));
        };

        bool is_decreasing = vec[1] < vec[0];
        if (!diff_is_safe(0, 1, is_decreasing)) {
            return can_safely_remove(1) || can_safely_remove(0);
        }

        for (std::size_t i = 2; i < vec.size(); ++i) {
            if (diff_is_safe(i - 1, i, is_decreasing)) {
                continue;
            }
            if (i == vec.size() - 1) {
                return true;
            }
            if (can_safely_remove(i, is_decreasing)) {
                return true;
            }
            if (can_safely_remove(i - 1, is_decreasing)) {
                return true;
            }
            if (i > 2) {
                return false;
            }
            return rest_is_safe(2, vec[2] < vec[1]);
        }
        return true;
    });
}

int main(int argc, char **argv) {
    printf("%d\n",
           ((std::stoi(argv[argc - 1]) - 1) ? solution2() : solution1()));
}
