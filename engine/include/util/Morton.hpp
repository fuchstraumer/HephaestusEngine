#pragma once
#ifndef MORTON_CODING_UTILITIES_HPP
#define MORTON_CODING_UTILITIES_HPP
#include <cstdint>
#include <immintrin.h>
#include <array>
#include <type_traits>

namespace detail {

    template<typename T>
    constexpr static T BMI_X_MASK = T(0x9249249249249249);
    template<typename T>
    constexpr static T BMI_Y_MASK = T(0x2492492492492492);
    template<typename T>
    constexpr static T BMI_Z_MASK = T(0x4924924924924924);

}

template<typename T>
static uint32_t morton_encode(const T x, const T y, const T z) {
    static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "Morton encoding type must be unsigned int type! (u32/u64)");
    if constexpr (std::is_same_v<T, uint32_t>) {
        return _pdep_u32(z, detail::BMI_X_MASK<T>) | _pdep_u32(y, detail::BMI_Y_MASK<T>) | _pdep_u32(x, detail::BMI_Z_MASK<T>);
    }
    else if constexpr (std::is_same_v<T, uint64_t>) {
        return _pdep_u64(z, detail::BMI_X_MASK<T>) | _pdep_u64(y, detail::BMI_Y_MASK<T>) | _pdep_u64(x, detail::BMI_Z_MASK<T>);
    }
}

template<typename T>
static void morton_decode(const T input_code, T& result_x, T& result_y, T& result_z) {
    static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "Morton decoding type must be unsigned int type! (u32/u64)");
    if constexpr (std::is_same_v<T, uint32_t>) {
        result_x = _pext_u32(input_code, detail::BMI_X_MASK<T>);
        result_y = _pext_u32(input_code, detail::BMI_Y_MASK<T>);
        result_z = _pext_u32(input_code, detail::BMI_Z_MASK<T>);
    }
    else if constexpr (std::is_same_v<T, uint64_t>) {
        result_x = _pext_u64(input_code, detail::BMI_X_MASK<T>);
        result_y = _pext_u64(input_code, detail::BMI_Y_MASK<T>);
        result_z = _pext_u64(input_code, detail::BMI_Z_MASK<T>);
    }
}


#endif //!MORTON_CODING_UTILITIES_HPP
