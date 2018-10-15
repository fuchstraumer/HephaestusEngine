#pragma once
#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

// Same as above, with individual positions
static constexpr inline size_t GetBlockIndex(const size_t& x, const size_t& y, const size_t& z) {
	return (y * CHUNK_SIZE * CHUNK_SIZE) + (x * CHUNK_SIZE) + z;
}

// Used for grabbing front 4 bits of a given uint8_t
inline int GetFront4(const uint8_t val) {
	return (val >> 4) & 0xF;
}

// Used for grabbing back 4 bits of a given uint8_t
inline int GetBack4(const uint8_t val) {
	return val & 0xF;
}

// Used for setting front 4 bits of given uint8_t to value int (doesn't check to make sure within range)
inline void SetFront4(uint8_t& dest, const int val) {
	dest = (dest & 0xF) | (val << 4);
}

// Used for setting back 4 bits of given uint8_t
inline void SetBack4(uint8_t& dest, const int val) {
	dest = (dest & 0xF0) | val;
}


#endif // !COMMON_UTIL_H
