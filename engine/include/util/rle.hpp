#pragma once
#ifndef RUN_LENGTH_ENCODING_HPP
#define RUN_LENGTH_ENCODING_HPP
#include <vector>
#include <algorithm>

template<typename T>
struct rle_system {

    static_assert(std::is_integral_v<T>, "Type used for run-length encoding must be an integral type!");

    using data_container = std::vector<T>;
    using data_iterator = data_container::const_iterator;

    constexpr static T COUNTER_BITS = std::numeric_limits<T>::max() / T(2);
    constexpr static T REPETITION_BIT = ~COUNTER_BITS;

    constexpr static auto count_repetitions(data_iterator iter, data_iterator end) noexcept {
        const auto first = iter;

        do ++iter;
        while (iter != end && *iter == *first);

        return std::distance(first, iter);
    }

    constexpr static auto count_uniques(data_iterator iter, data_iterator end) noexcept {
        const auto first = iter;
        // Make sure to check the range
        // while not at the end and while the value of iter + 1
        // isn't equal to the current value of iter
        while ((iter + 1) != end && *(iter + 1) != *iter)
            ++iter;
        return std::distance(first, iter);
    }

    // This divides our N-items into chunks of the maximum size permitted by counter bits
    template<typename SplitFn>
    static auto split(std::ptrdiff_t num, SplitFn&& t) {
        do {
            // Our count is the smallest of these two. This is only okay because we always know 
            // our count can never be negative. At worst, it'll be 0 or 1.
            auto count = std::min(static_cast<unsigned int>(num), static_cast<unsigned int>(COUNTER_BITS));
            t(count);
            num -= count;
        } while (num > 0);
    }

    static data_container encode(const data_container &input) {
        data_container out;
        out.reserve(input.size());

        for (auto iter = input.cbegin(); iter != input.cend();) {
            // First attempt at checking for repetitions
            auto num = count_repetitions(iter, input.cend());
            // If there's more than one repetition, we have a run
            if (num > 1) {
                split(num, [&](T count) {
                    // Repetition bit plus the count
                    out.emplace_back(REPETITION_BIT | count);
                    // Insert the value that's being repeated
                    out.emplace_back(*iter);
                });
                // Iterate iter by the number of repetitions of *iter
                iter += num;
            }
            else {
                // Count amount of non-repeated characters in a row/run (unique chars)
                auto num = count_uniques(iter, input.cend());
                split(num, [&](T count) {
                    // No repetition bit, just counter
                    out.emplace_back(count);
                    std::copy(iter, iter + count, std::back_inserter(out));
                    iter += count;
                });
            }
        }

        out.shrink_to_fit();
        return out;
    }

    static data_container decode(const data_container &compressed_input) {
        data_container out;
        // Over-estimate potential size
        out.reserve(compressed_input.size() * 4);

        for (auto iter = compressed_input.cbegin(); iter != compressed_input.cend();) {

            bool repeat = (*iter & REPETITION_BIT) > 0;

            auto count = *iter & COUNTER_BITS;

            if (repeat) {
                for (auto i = 0; i < count;) {
                    out.emplace_back(*iter);
                }
                ++iter;
            }
            else {
                ++iter;
                for (; count > 1; count--) {
                    out.emplace_back(*++iter);
                }
            }

        }

        out.shrink_to_fit();
        return out;
    }

};

#endif // !RUN_LENGTH_ENCODING_HPP
