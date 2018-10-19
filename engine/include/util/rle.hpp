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

    constexpr static T counterBits = std::numeric_limits<T>::max() / T(2);
    constexpr static T repetitionBit = ~counterBits;

    constexpr static auto count_repetitions(data_iterator iter, data_iterator end) {
        const auto first = iter;

        do ++iter;
        while (iter != end && *iter == *first);

        return std::distance(first, iter);
    }

    constexpr static auto count_uniques(data_iterator iter, data_iterator end) {
        const auto first = iter;
        // Make sure to check the range
        // while not at the end and while the value of iter + 1
        // isn't equal to the current value of iter
        while ((iter + 1) != end && *(iter + 1) != *iter)
            ++iter;
        return std::distance(first, iter);
    }

    // This divides our N-items into chunks of the maximum size permitted by counter bits
    template<typename T>
    static auto split(std::ptrdiff_t num, T&& t) {
        do {
            // Our count is the smallest of these two. This is only okay because we always know 
            // our count can never be negative. At worst, it'll be 0 or 1.
            auto count = std::min(static_cast<unsigned int>(num), static_cast<unsigned int>(counterBits));
            t(count);
            num -= count;
        } while (num > 0);
    }

    static data_container encode(const data_container &data) {
        data_container out;
        out.reserve(data.size());
        for (auto iter = data.cbegin(); iter != data.cend();) {
            // First attempt at checking for repetitions
            auto num = count_repetitions(iter, data.cend());
            // If there's more than one repetition, we have a run
            if (num > 1) {
                split(num, [&](T count) {
                    // Repetition bit plus the count
                    out.emplace_back(repetitionBit | count);
                    // Insert the value that's being repeated
                    out.emplace_back(*iter);
                });
                // Iterate iter by the number of repetitions of *iter
                iter += num;
            }
            else {
                // Count amount of non-repeated characters in a row/run (unique chars)
                auto num = count_uniques(iter, data.cend());
                split(num, [&](T count) {
                    // No repeition bit, just counter
                    out.emplace_back(count);
                    // Instead of having to loop over the iter, we can
                    // just copy values from one vector to another.
                    std::copy(iter, iter + count, std::back_inserter(out));
                    // Iterate down the data by how many unique chars we found
                    iter += count;
                });
            }
        }
        out.shrink_to_fit();
        return out;
    }

    static data_container decode(const data_container &data) {
        data_container out; // No good way to reserve - can't guess 
        // size of decompressed data very easily.
        for (auto iter = data.cbegin(); iter != data.cend();) {
            bool repeat = (*iter & repetitionBit) > 0;
            auto count = *iter & counterBits;
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
        return out;
    }

};

#endif // !RUN_LENGTH_ENCODING_HPP
