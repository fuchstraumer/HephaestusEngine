#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ECS_SORT_ALGORITHMS_HPP
#define HEPHAESTUS_ENGINE_CORE_ECS_SORT_ALGORITHMS_HPP
#include <functional>
#include <algorithm>

namespace ecs {

    struct std_sort {
        template<typename Iter, typename Compare = std::less<>, typename...Args>
        void operator()(Iter first, Iter last, Compare cmp = Compare{}, Args&&...args) const {
            std::sort(std::forward<Args>(args)..., std::move(first), std::move(last), std::move(cmp));
        }
    };

    struct insertion_sort {
        template<typename Iter, typename Compare = std::less<>>
        void operator()(Iter first, Iter last, Compare cmp = Compare{}) const {
            auto iter = first + 1;
            while (iter != last) {
                auto value = *iter;
                auto pre = iter;

                while (pre != first && cmp(value, *(pre - 1))) {
                    *pre = *(pre - 1);
                    --pre;
                }

                *pre = value;
                ++iter;
            }
        }
    };

}

#endif //!HEPHAESTUS_ENGINE_CORE_ECS_SORT_ALGORITHMS_HPP
