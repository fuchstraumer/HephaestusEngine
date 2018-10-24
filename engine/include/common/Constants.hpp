#pragma once
#ifndef HEPHAESTUS_ENGINE_CONSTANTS_HPP
#define HEPHAESTUS_ENGINE_CONSTANTS_HPP

// Light has a range from 0-15. 15 is absolute highest, equivalent to sunlight.
// 14 is highest we allow for all non-sunlight sources.
static constexpr int SUNLIGHT_LEVEL = 15;
static constexpr int MAX_LIGHT_INTENSITY = 14;

/*
    These values should not be runtime-modifiable and should be set
    based on profiling/optimization based on block size.
*/
static constexpr size_t CHUNK_SIZE = 32;
static constexpr size_t CHUNK_SIZE_Y = 128;
static constexpr size_t Z_BLOCK_STRIDE = CHUNK_SIZE * CHUNK_SIZE;
static constexpr size_t X_BLOCK_STRIDE = CHUNK_SIZE;
static constexpr size_t BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE_Y;

enum class BlockFace : unsigned char {
    FRONT,
    RIGHT,
    TOP,
    LEFT,
    BOTTOM,
    BACK,
};

#endif // !HEPHAESTUS_ENGINE_CONSTANTS_HPP
