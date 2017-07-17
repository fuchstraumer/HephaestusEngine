#pragma once
#ifndef BLOCK_H
#define BLOCK_H
#include "stdafx.h"
#include "common\Constants.h"

namespace objects {

	/*

	Struct - Block

	Represents a single block in the game world. The main item loaded in chunks.

	Original source: https://github.com/minetest/minetest/blob/master/src/mapnode.h

	*/

	// Range of potential rotations a block can experience.
	enum class BlockRotation : uint8_t {
		R0,
		R90,
		R180,
		R270,
		// Random rotation. Useful for rendering things like grass/decorative gameworld elements.
		R_RAND,
	};

	class Block {
	public:

		Block() = default;
		Block(uint8_t block_type, uint8_t parameter = 0, BlockRotation rot = BlockRotation::R0);

		Block(const Block& other);
		Block& operator=(const Block& other);

		Block(Block&& other) noexcept;
		Block& operator=(Block&& other) noexcept;

		bool operator==(const Block& other) const;
		uint8_t GetType() const;

		void SetType(const uint8_t& new_data);
		void SetType(uint8_t&& new_data);

		uint8_t GetParam() const;
		void SetParam(const uint8_t& new_param);
		void SetParam(uint8_t&& new_param);

		BlockRotation GetRotation() const;
		void SetRotation(const BlockRotation& new_rot);
		void SetRotation(BlockRotation&& new_rot);

		bool Opaque() const;

	private:

		// main data. Does not vary in content across block types.
		uint8_t type;

		// Parameter dataset. 
		// Used for special blocks, like torches and flowing water.
		uint8_t parameters;

		BlockRotation rotation;

	};

	enum class BlockDrawMode : uint8_t {
		AIR, // An air block, i.e don't draw it at all
		NORMAL, // A solid block
		LIQUID, // Liquid block. Partially opaque.
		GLASS, // Glass block, or glass-like block.
		PLANT, // Draw this block like it was a plant
		LEAVES, // Draw this block like its a leaf block
	};

	// Type of data kept in the parameter data item
	enum class ParameterData : uint8_t {
		NONE, // Nothing, no need to use param
		FULL, // Need full space of param.
		FACE_DIRECTION, // Used to set proper direction: important for blocks like chests
		DIRECTION, // Not like face direction: sets more granular direction. used for plants.
		MESH_OPTIONS, // Used with plant rendering, leave rendering, etc
	};

	/*
	
		Struct - BlockTypeDescription

		This struct is initialized with a BlockType value. Using this, it sets its 
		internal values appropriately upon init and can then be used for querying
		the properties of a certain blocktype.
	
	*/
	struct BlockTypeDescription {

		BlockTypeDescription(uint16_t block_type);

		/*
			Base properties.
		*/

		// Whether or not the block type is solid
		bool Solid : 1;

		// Whether or not this block type can be built upon
		bool BuildableOn : 1;

		// Whether or not this object can be placed/destroyed
		bool Invulnerable : 1;

		// Whether or not this object can be highlighted in-game
		bool Selectable : 1;

		// What kind of data does this block type store in param1?
		ParameterData ParamDataInfo;

		/*
			Lighting properties.
		*/

		// Should light propagate through this block?
		bool PropagateLight : 1;

		// How much light does this type of block emit (if it does)?
		uint8_t LightEmissionIntensity;

		/*
			Rendering information
		*/

		// How to render this type of block
		BlockDrawMode DrawMode;
		
		/*
			Information used by the terrain generator
		*/

		// Is this block part of the terrain? (if not, probably leaves, plants, structure, etc)
		bool TerrainElement : 1;

		// Is this block a resource block?
		bool Resource : 1;

	};


	// Ray-Intersection routine. Mainly used to check for collisions.


}
#endif // !BLOCK_H
