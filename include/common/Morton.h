#pragma once
#ifndef MORTON_UTILITIES_H
#define MORTON_UTILITIES_H
#include "stdafx.h"

namespace morton {

	constexpr uint32_t BMI_X_MASK = 0x9249249249249249;
	constexpr uint32_t BMI_Y_MASK = 0x2492492492492492;
	constexpr uint32_t BMI_Z_MASK = 0x4924924924924924;

	constexpr static uint32_t encode(const uint32_t& x, const uint32_t& y, const uint32_t& z) {
		uint32_t result = 0;
		result |= _pdep_u32(x, BMI_X_MASK);
		result |= _pdep_u32(y, BMI_Y_MASK);
		result |= _pdep_u32(z, BMI_Z_MASK);
		return result;
	}

	constexpr static void decode(const uint32_t& input_code, uint32_t& result_x, uint32_t& result_y, uint32_t& result_z) {
		result_x = _pext_u32(input_code, BMI_X_MASK);
		result_y = _pext_u32(input_code, BMI_Y_MASK);
		result_z = _pext_u32(input_code, BMI_Z_MASK);
	}

}
#endif // !MORTON_UTILITIES_H
