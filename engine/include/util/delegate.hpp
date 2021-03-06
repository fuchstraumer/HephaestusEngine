#pragma once
#ifndef TETHERSIM2X_CORE_DELEGATE_HPP
#define TETHERSIM2X_CORE_DELEGATE_HPP

template<typename T>
class base_delegate_t;

template<typename Result, typename...Args>
class base_delegate_t<Result(Args...)> {
protected:
    using func_stub_t = Result(*)(void* this_ptr, Args...);

    struct invocation_element_t {
        invocation_element_t() = default;
        invocation_element_t(void* this_ptr, func_stub_t _stub) : object(this_ptr), stub(_stub) {}

        invocation_element_t(const invocation_element_t& other) noexcept : object(other.object), 
            stub(other.stub) {}

        invocation_element_t(invocation_element_t&& other) noexcept : object(std::move(other.object)),
            stub(std::move(other.stub)) {}

        invocation_element_t& operator=(const invocation_element_t& other) noexcept {
            object = other.object;
            stub = other.stub;
            return *this;
        }

        invocation_element_t& operator=(invocation_element_t&& other) noexcept {
            object = std::move(other.object);
            stub = std::move(other.stub);
            return *this;
        }

        void copy_to(invocation_element_t& destination) const noexcept {
            destination.stub = stub;
            destination.object = object;
        }

        bool operator==(const invocation_element_t& other) const noexcept {
            return (other.stub == stub) && (other.object == object);
        }

        bool operator!=(const invocation_element_t& other) const noexcept {
            return (other.stub != stub) || (other.object != object);
        }

        void* object{ nullptr };
        func_stub_t stub{ nullptr };
    };
};

template<typename T>
class delegate_t;

template<typename T>
class multicast_delegate_t;

template<typename Result, typename...Args>
class delegate_t<Result(Args...)> final : private base_delegate_t<Result(Args...)> {
    friend class multicast_delegate_t<Result(Args...)>;
public:
    
    delegate_t() noexcept = default;

    delegate_t(const delegate_t& other) noexcept : invocation(other.invocation) {}
    delegate_t(delegate_t&& other) noexcept : invocation(std::move(other.invocation)) {}

    delegate_t& operator=(const delegate_t& other) noexcept {
        invocation = other.invocation;
        return *this;
    }

    delegate_t& operator=(delegate_t&& other) noexcept {
        invocation = std::move(other.invocation);
        return *this;
    }

    bool IsEmpty() const noexcept {
        return invocation.stub == nullptr;
    }

    operator bool() const noexcept {
        return invocation.stub != nullptr;
    }

    bool operator==(const void* ptr) const noexcept {
        return (ptr == nullptr) && (IsEmpty());
    }

    bool operator!=(const void* ptr) const noexcept {
        return (ptr != nullptr) || (!IsEmpty());
    }

    template<typename LambdaFunc>
    delegate_t(const LambdaFunc& func) {
        assign((void*)(&func), lambda_stub<LambdaFunc>);
    }

    template<typename LambdaFunc>
    delegate_t& operator=(const LambdaFunc& func) {
        assign((void*)(&func), lambda_stub<LambdaFunc>);
        return *this;
    }

    // i.e like if (!ptr_object), returns true when object doesn't "exist" and false
    // when it exists. thus if (!my_delegate_t) == if (my_delegate_t.invocation.stub == nullptr)
    bool operator!() const noexcept {
        return invocation.stub == nullptr;
    }

    bool operator==(const delegate_t& other) const noexcept {
        return invocation == other.invocation;
    }

    bool operator!=(const delegate_t& other) const noexcept {
        return invocation != other.invocation;
    }

    Result operator()(Args...args) const {
        return (*invocation.stub)(invocation.object,args...);
    }

    template<class T, auto Member>
    static delegate_t create(T* object) {
        return delegate_t(object, method_stub<T, Member>);
    }

    template<class T, auto Member>
    static delegate_t create(const T* object) {
        return delegate_t(const_cast<T*>(object), method_stub<T, Member>);
    }

    template<Result(*Function)(Args...)>
    static delegate_t create() {
        return delegate_t(nullptr, function_stub<Function>);
    }

    template<typename LambdaFunc>
    static delegate_t create(const LambdaFunc& lambda) {
        return delegate_t((void*)(&lambda), lambda_stub<LambdaFunc>);
    }

private:

    delegate_t(void* obj, typename base_delegate_t<Result(Args...)>::func_stub_t stub) {
        this->invocation.object = obj;
        this->invocation.stub = stub;
    }

    void assign(void* object_ptr, typename base_delegate_t<Result(Args...)>::func_stub_t _stub) {
        this->invocation.object = object_ptr;
        this->invocation.stub = _stub;
    }

    template<typename Class, auto Member>
    static Result method_stub(void* this_ptr, Args...args) {
        return std::invoke(Member, static_cast<Class*>(this_ptr), args...);
    }

    template<auto Function>
    static Result function_stub(void*, Args...args) {
        return std::invoke(Function, args...);
    }

    template<typename LambdaFunc>
    static Result lambda_stub(void* this_ptr, Args...args) {
        // this_ptr is a lambda func pointer, deference it so std::invoke can call it.
        return std::invoke(*static_cast<LambdaFunc*>(this_ptr), args...);
        
    }

    typename base_delegate_t<Result(Args...)>::invocation_element_t invocation{};

};

#endif //!TETHERSIM2X_CORE_DELEGATE_HPP
