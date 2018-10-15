#pragma once
#ifndef TETHERSIM_MULTICAST_DELEGATE_HPP
#define TETHERSIM_MULTICAST_DELEGATE_HPP
#include "delegate.hpp"
#include <forward_list>
#include <memory>

namespace tethersim {

    template<typename Result, typename...Args>
    class multicast_delegate_t<Result(Args...)> final : private base_delegate_t<Result(Args...)> {
        multicast_delegate_t(const multicast_delegate_t&) = delete;
        multicast_delegate_t& operator=(const multicast_delegate_t&) = delete;
        friend class delegate_t<Result(Args...)>;

        // alias this here to save typing. "typename" required for derived type
        using invocation_type_t = typename base_delegate_t<Result(Args...)>::invocation_element_t;

    public:

        multicast_delegate_t() = default;

        ~multicast_delegate_t() {
            invocationList.clear();
        }

        operator bool() const noexcept {
            return !invocationList.empty();
        }

        size_t size() const noexcept {
            // forward_list has no size() function, this is best we can do and should work 99.9% of the time
            return std::distance(invocationList.cbegin(), invocationList.cend());
        }

        void clear() noexcept {
            invocationList.clear();
        }

        void remove(const delegate_t<Result(Args...)>& fn) {
            // removes function if invocation signatures match
            invocationList.remove_if(fn.invocation);
        }

        bool operator==(const multicast_delegate_t& other) const {
            if (size() != other.size()) {
                return false;
            }

            auto other_iter = other.invocationList.cbegin();
            for (auto iter = invocationList.cbegin(); iter != invocationList.cend(); ++iter) {
                if (**iter != **other_iter) {
                    return false;
                }
            }
        }

        bool operator!=(const multicast_delegate_t& other) const {
            return !(*this == other);
        }

        multicast_delegate_t& operator+=(const multicast_delegate_t& other) {
            for (const auto& item : other.invocationList) {
                invocationList.emplace_front(std::make_unique<invocation_type_t>(item->object, item->stub));
            }
            return *this;
        }

        template<typename LambdaFunc>
        multicast_delegate_t& operator+=(const LambdaFunc& lambda) {
            delegate_t<Result(Args...)> d = delegate_t<Result(Args...)>::template create<LambdaFunc>(lambda);
            return *this += d;
        }

        multicast_delegate_t& operator+=(const delegate_t<Result(Args...)>& other) {
            if (!other) {
                return *this;
            }
            invocationList.emplace_front(std::make_unique<invocation_type_t>(other.invocation.object, other.invocation.stub));
            return *this;
        }

        void operator()(Args...args) const {
            for (auto& item : invocationList) {
                (*(item->stub))(item->object, args...);
            }
        }

        template<typename Handler>
        void operator()(Args...args, Handler handler) const {
            size_t idx = 0;
            for (auto& item : invocationList) {
                Result val = (*(item->stub))(item->object, args...);
                handler(idx, &val);
                ++idx;
            }
        }

        void operator()(Args...args, delegate_t<void(size_t, Result*)> handler) const {
            operator()<decltype(handler)>(args..., handler);
        }

    private:
        // using forward_list for it's minimal overhead w/ regards to size, and potential
        // improved iteration performance
        std::forward_list<std::unique_ptr<invocation_type_t>> invocationList;
    };

}

#endif //!TETHERSIM_MULTICAST_DELEGATE_HPP
