#pragma once
#ifndef MORTON_UTILITIES_H
#define MORTON_UTILITIES_H
#include <cstdint>
#include <immintrin.h>

constexpr static uint32_t BMI_X_MASK = 0x09249249;
constexpr static uint32_t BMI_Y_MASK = 0x12492492;
constexpr static uint32_t BMI_Z_MASK = 0x24924924;

static uint32_t morton_encode(const uint32_t x, const uint32_t y, const uint32_t z) {
    return _pdep_u32(z, BMI_X_MASK) | _pdep_u32(y, BMI_Y_MASK) | _pdep_u32(x, BMI_Z_MASK);
}

static void morton_decode(const uint32_t input_code, uint32_t& result_x, uint32_t& result_y, uint32_t& result_z) {
    result_x = _pext_u32(input_code, BMI_X_MASK);
    result_y = _pext_u32(input_code, BMI_Y_MASK);
    result_z = _pext_u32(input_code, BMI_Z_MASK);
}


#endif // !MORTON_UTILITIES_H
