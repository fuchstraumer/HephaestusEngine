#pragma once
#ifndef BLOCK_H
#define BLOCK_H
#include "stdafx.h"

namespace objects {

	// Range of potential rotations a block can experience.
	enum class BlockRotation {
		R0,
		R90,
		R180,
		R270,
		// Random rotation. Useful for rendering things like grass/decorative gameworld elements.
		R_RAND,
	};

	struct Block {

		/*

			Struct - Block

			Represents a single rendered block in the game world. The main item loaded in chunks.

			There are other renderable objects, but those are encompassed by other classes/structs,
			as this struct makes up the predominant object type in a game world.

			We use large ints/uints to hold a variety of data on these objects, and then use
			masking operations to access seperate attributes or set seperate attributes. This 
			lets us enjoy space-efficient storage of a number of diverse attributes. 

			Original source: https://github.com/minetest/minetest/blob/master/src/mapnode.h

		*/

		// Constructor
		Block(blockType type);

		// Default constructor: sets Data to all 0's, doesn't set type. Getter function assumes that nullptr "type" means
		// type is just air.
		Block();

		~Block();

		// Copy ctor
		Block(const Block& other);

		// Copy operator
		Block& operator=(const Block& other);

		// Move ctor
		Block(Block&& other);

		// Move operator
		Block& operator=(Block&& other);

		// Sets physical status
		void SetPhysics(bool physics);

		// Resets block data.
		void Reset();

		void SetType(blockType _type);

		// Returns whether or not this block is active.
		const bool Active() const;

		// Returns whether or not this block is a opaque
		const bool Opaque() const;

		// Returns whether or not this block needs to be physically simulated
		const bool Physics() const;

		// Returns block type
		const blockType& GetType() const;

	private:

		// main data. Does not vary in content across block types.
		uint16_t data0;

		// dataset 1. Stores light values, usually. Currently nothing else here,
		// but could be used for other things I guess.
		uint8_t data1;

		// dataset 2. 

	};

	// Following are utility algorithms. Things like flood fills for light, ambient occlusion calculators, and so on.

}
#endif // !BLOCK_H
