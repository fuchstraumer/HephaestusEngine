#pragma once
#ifndef MORTON_UTILITIES_H
#define MORTON_UTILITIES_H
#include "stdafx.h"

namespace morton {

	constexpr uint32_t BMI_X_MASK = 0x9249249249249249;
	constexpr uint32_t BMI_Y_MASK = 0x2492492492492492;
	constexpr uint32_t BMI_Z_MASK = 0x4924924924924924;

	static uint32_t encode(const uint32_t x, const uint32_t y, const uint32_t z) {
		uint32_t result = 0;
		result |= _pdep_u32(x, BMI_X_MASK);
		result |= _pdep_u32(y, BMI_Y_MASK);
		result |= _pdep_u32(z, BMI_Z_MASK);
		return result;
	}

	static void decode(const uint32_t input_code, uint32_t& result_x, uint32_t& result_y, uint32_t& result_z) {
		result_x = _pext_u32(input_code, BMI_X_MASK);
		result_y = _pext_u32(input_code, BMI_Y_MASK);
		result_z = _pext_u32(input_code, BMI_Z_MASK);
    }

    /*
        The following look-up-tables are used to check a distance from a point
        without incurring a heavy computational expense
    */

    constexpr static std::array<int32_t, 256> deltaX = { 
        1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 
        3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 
        7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 
        55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 
        7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 
        439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 
        7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 
        55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 
        7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 1797559, 
        1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 
        3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 
        7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 
        55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 
        7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 
        439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 
        7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 
        55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 
        7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1   
    };

    static const std::array<int32_t, 256> deltaY = {
        2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 
        14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 
        2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 
        878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 
        2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 
        14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 
        7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 
        2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 
        14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 
        2, 878, 2, 14, 2, 110, 2, 14, 2, 3595118, 2, 14, 
        2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 
        7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110,
        2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 
        14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 
        2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2, 
        110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 
        2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 
        2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 
        110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 
        2, 14, 2, 110, 2, 14, 2		
    };

    static const std::array<int32_t, 256> deltaZ = {
        4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 
        28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 
        28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 
        28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 
        28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 
        4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 
        4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 
        1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4,
        220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 
        4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 
        28, 4, 7190236, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 
        28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 
        4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 
        220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 
        4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 
        4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 
        220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 
        4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 
        4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 
        4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4		
    };

    constexpr inline auto NegXDelta(const size_t x) {
        return -deltaX[x - 1];
    }

    constexpr inline auto PosXDelta(const size_t x) {
        return deltaX[x];
    }

    constexpr inline auto NegYDelta(const size_t y) {
        return -deltaY[y - 1];
    }

    constexpr inline auto PosYDelta(const size_t y) {
        return deltaY[y];
    }

    constexpr inline auto NegZDelta(const size_t z) {
        return -deltaZ[z - 1];
    }

    constexpr inline auto PosZDelta(const size_t z) {
        return deltaZ[z];
    }

}
#endif // !MORTON_UTILITIES_H