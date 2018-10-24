#pragma once
#ifndef HEPHAESTUS_ENGINE_BLOCK_HPP
#define HEPHAESTUS_ENGINE_BLOCK_HPP
#include "common/Constants.hpp"

// Range of potential rotations a block can experience.
enum class BlockRotation : uint8_t {
	R0,
	R90,
	R180,
	R270,
	// Random rotation. Useful for rendering things like grass/decorative gameworld elements.
	R_RAND,
};

struct BlockComponent {
    uint16_t Type;
    BlockRotation Rotation;
    // mostly added as padding
    uint8_t Parameters;
};

#endif // !BLOCK_H
