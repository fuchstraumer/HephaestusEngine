#pragma once
#ifndef BLOCK_H
#define BLOCK_H
#include "stdafx.h"

namespace objects {

	struct Block {

		/*

			Struct - Block

			Uses a bitset to store 4 attributes
			we care about for rendering/working with
			blocks in this engine.

			Stores type of block.

			4 attributes:

			Data[0] = Active/Inactive. Whether this block is even rendered: if inactive, the block is obscured
					  by other blocks and shouldn't be visible.
			Data[1] = transparent/opaque object. Used to set correct rendering order for alpha blending, and
			          used in lighting tests.
			Data[2] = physics object - specifies we need to simulate the physics of this object.
			Data[3] = unused, for now.

			Position of a block is based on its position in the unordered map of special blocks (first part of entry
			defining a block, second part is this class).

		*/

		// Constructor
		Block(blockType type);

		// Default constructor: sets Data to all 0's, doesn't set type. Getter function assumes that nullptr "type" means
		// type is just air.
		Block();

		// Destructor
		~Block();

		// Copy ctor
		Block(const Block& other);

		// Copy operator
		Block& operator=(const Block& other);

		// Move ctor
		Block(Block&& other);

		// Move operator
		Block& operator=(Block&& other);

		// Sets active status
		void SetActive(bool active);

		// Sets opaque status
		void SetOpaque(bool opaque);

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

		std::bitset<4> data;
		blockType* type;
	};

}
#endif // !BLOCK_H
