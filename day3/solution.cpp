#include "../performance/timer.h"

#include <algorithm>
#include <array>
#include <climits>
#include <cstring>
#include <numeric>
#include <optional>
#include <vector>

std::string read_from_stdin() {
    std::string res = "", line;
    while (std::getline(std::cin, line)) {
        res += line;
    }
    return res;
}

enum class State {
    BASE = 0,
    M,
    U,
    L,
    LPAREN,
    DIG1_1,
    DIG1_2,
    DIG1_3,
    COMMA,
    DIG2_1,
    DIG2_2,
    DIG2_3,
    RPAREN,

    P2_D,
    P2_O,
    P2_N,
    P2_APPROS,
    P2_T,
    P2_DONT_LPAREN,
    P2_DONT_RPAREN,
    P2_DO_LPAREN,
    P2_DO_RPAREN,

    MAX = State::P2_DO_RPAREN + 1
};

enum class DigitPlace {
    NO_DIGIT,
    DIG1_1,
    DIG1_2,
    DIG1_3,
    COMMA,
    DIG2_1,
    DIG2_2,
    DIG2_3,
};

struct StateMachineEntry {
    State next_state;
    DigitPlace digit_place;
};

using Machine = std::array<std::array<StateMachineEntry, 128>,
                           static_cast<int>(State::MAX) + 1>;

using DigitsArray = std::array<char, 8>;

constexpr Machine state_machine = ([]() constexpr {
    Machine machine{};
    auto set_transition = [&machine](State state, char chr, State next_state,
                                     DigitPlace digit =
                                         DigitPlace::NO_DIGIT) constexpr {
        machine[static_cast<int>(state)][chr] = {next_state, digit};
    };

    constexpr char ANY_DIGIT = '#';

    auto add_path = [&]<typename T>(T path) -> void {
        State state = State::BASE;
        for (const auto &[chr, next_state, digit_action] : path) {
            if (chr == ANY_DIGIT) {
                for (char c = '0'; c <= '9'; ++c) {
                    set_transition(state, c, next_state,
                                   digit_action.value_or(DigitPlace::NO_DIGIT));
                }
            } else {
                set_transition(state, chr, next_state,
                               digit_action.value_or(DigitPlace::NO_DIGIT));
            }
            state = next_state;
        }
    };

    std::array<std::tuple<char, State, std::optional<DigitPlace>>, 12> mul_path{
        {
            {'m', State::M, {}},
            {'u', State::U, {}},
            {'l', State::L, {}},
            {'(', State::LPAREN, {}},
            {ANY_DIGIT, State::DIG1_1, DigitPlace::DIG1_1},
            {ANY_DIGIT, State::DIG1_2, DigitPlace::DIG1_2},
            {ANY_DIGIT, State::DIG1_3, DigitPlace::DIG1_3},
            {',', State::COMMA, {}},
            {ANY_DIGIT, State::DIG2_1, DigitPlace::DIG2_1},
            {ANY_DIGIT, State::DIG2_2, DigitPlace::DIG2_2},
            {ANY_DIGIT, State::DIG2_3, DigitPlace::DIG2_3},
            {')', State::RPAREN, {}},
        }};

    add_path(mul_path);
    set_transition(State::DIG1_1, ',', State::COMMA);
    set_transition(State::DIG1_2, ',', State::COMMA);
    set_transition(State::DIG2_1, ')', State::RPAREN);
    set_transition(State::DIG2_2, ')', State::RPAREN);

    auto handle_part2 = [&]() -> void {
        std::array<std::tuple<char, State, std::optional<DigitPlace>>, 4>
            do_path{{
                {'d', State::P2_D, {}},
                {'o', State::P2_O, {}},
                {'(', State::P2_DO_LPAREN, {}},
                {')', State::P2_DO_RPAREN, {}},
            }};

        std::array<std::tuple<char, State, std::optional<DigitPlace>>, 7>
            dont_path{{
                {'d', State::P2_D, {}},
                {'o', State::P2_O, {}},
                {'n', State::P2_N, {}},
                {'\'', State::P2_APPROS, {}},
                {'t', State::P2_T, {}},
                {'(', State::P2_DONT_LPAREN, {}},
                {')', State::P2_DONT_RPAREN, {}},
            }};
        add_path(do_path);
        add_path(dont_path);
    };
    handle_part2();

    for (size_t i = 0; i < machine.size(); ++i) {
        machine[i]['m'] = {State::M, DigitPlace::NO_DIGIT};
        machine[i]['d'] = {State::P2_D, DigitPlace::NO_DIGIT};
    }
    return machine;
})();

StateMachineEntry query_state_machine(State state, char chr) {
    return state_machine[static_cast<int>(state)][static_cast<int>(chr)];
}

long get_product(const DigitsArray &digits) {
    auto get_digit = [&](DigitPlace digit) -> char {
        return digits[static_cast<int>(digit)];
    };
    long num1 = get_digit(DigitPlace::DIG1_2) == 0
                    ? get_digit(DigitPlace::DIG1_1) - '0'
                : get_digit(DigitPlace::DIG1_3) == 0
                    ? (get_digit(DigitPlace::DIG1_1) - '0') * 10 +
                          (get_digit(DigitPlace::DIG1_2) - '0')
                    : (get_digit(DigitPlace::DIG1_1) - '0') * 100 +
                          (get_digit(DigitPlace::DIG1_2) - '0') * 10 +
                          (get_digit(DigitPlace::DIG1_3) - '0');
    long num2 = get_digit(DigitPlace::DIG2_2) == 0
                    ? get_digit(DigitPlace::DIG2_1) - '0'
                : get_digit(DigitPlace::DIG2_3) == 0
                    ? (get_digit(DigitPlace::DIG2_1) - '0') * 10 +
                          (get_digit(DigitPlace::DIG2_2) - '0')
                    : (get_digit(DigitPlace::DIG2_1) - '0') * 100 +
                          (get_digit(DigitPlace::DIG2_2) - '0') * 10 +
                          (get_digit(DigitPlace::DIG2_3) - '0');
    return num1 * num2;
}

void reset_digits_array(DigitsArray &digits) {
    digits = {0, 0, 0, 0, 0, 0, 0, 0};
}

long solution1() {
    auto data = read_from_stdin();
    Timer start;
    DigitsArray digits;
    State state = State::BASE;
    return std::accumulate(
        data.cbegin(), data.cend(), 0l, [&](long acc, char chr) -> long {
            auto entry = query_state_machine(state, chr);
            digits[static_cast<int>(entry.digit_place)] = chr;
            state = entry.next_state;
            switch (state) {
            case State::LPAREN: {
                reset_digits_array(digits);
            } break;
            case State::RPAREN: {
                acc += get_product(digits);
            } break;
                [[likely]] default : {
                    break;
                };
            };
            return acc;
        });
}

long solution2() {
    auto data = read_from_stdin();
    Timer start;
    DigitsArray digits;
    State state = State::BASE;
    bool enabled = true;
    return std::accumulate(
        data.begin(), data.end(), 0l, [&](long acc, char chr) -> long {
            auto entry = query_state_machine(state, chr);
            digits[static_cast<int>(entry.digit_place)] = chr;
            state = entry.next_state;
            switch (state) {
            case State::LPAREN: {
                reset_digits_array(digits);
            } break;
            case State::RPAREN: {
                if (!enabled) {
                    break;
                }
                acc += get_product(digits);
            } break;
            case State::P2_DONT_RPAREN: {
                enabled = false;
            } break;
            case State::P2_DO_RPAREN: {
                enabled = true;
            } break;
                [[likely]] default : break;
            };
            return acc;
        });
    return 0;
}

int main(int argc, char **argv) {
    printf("%ld\n",
           ((std::stoi(argv[argc - 1]) - 1) ? solution2() : solution1()));
}
