#pragma once
#ifndef BLOCK_H
#define BLOCK_H
#include "stdafx.h"
#include "common\Constants.h"

namespace objects {

	/*

	Struct - Block

	Represents a single block in the game world. The main item loaded in chunks.

	There are other objects, but those are encompassed by other classes/structs,
	as this struct makes up the predominant object type in a game world.

	We use ints/uints to hold a variety of data on these objects, and then use
	masking operations to access seperate attributes or set seperate attributes. This
	lets us enjoy space-efficient storage of a number of diverse attributes.

	When we want to know more about a block, we pass its main data to a struct that
	grabs the rest of the attributes based on the type of a block. There are a large
	number of attributes, and this would otherwise take up a ton of space, so we try
	to avoid doing this unless we absolutely need to (like for rendered blocks).

	Original source: https://github.com/minetest/minetest/blob/master/src/mapnode.h

	*/

	class Block {

		// Constructors / operators

		// Constructs a block using a blocktype, optionally supplying parameter data
		Block(uint8_t block_type, uint8_t parameter = 0);

		// Copy ctor
		Block(const Block& other);

		// Copy operator
		Block& operator=(const Block& other);

		// Move ctor
		Block(Block&& other);

		// Move operator
		Block& operator=(Block&& other);

		// Comparison operator
		bool operator==(const Block& other) const;

		// Returns main data
		uint16_t GetData() const;

		// Sets main data
		void SetData(const uint16_t& new_data);

		// Returns param data
		uint8_t GetParam() const;

		// Sets param data
		void SetParam(const uint8_t& new_param);

	private:

		// main data. Does not vary in content across block types.
		uint16_t data;

		// Parameter dataset. 
		// Used for special blocks, like torches and flowing water.
		uint8_t param;

	};

	/*

		Following are block features and attributes. These are retrieved by the dedicated
		getter struct, which takes a blocktype in its ctor (uint8_t defining type of a block)
		and then can fetch the following features easily.

		It also tells us what the contents of param will be used for, for a given
		type of block.

	*/

	/* 
		How to draw a block. Used for setting mesh parameters, along 
		with generating the mesh data itself. In the case of glass blocks,
		for example, we make sure we draw the back face of each glass block
		but never a face touching another glass block.
	*/
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

	// Range of potential rotations a block can experience.
	enum class BlockRotation : uint8_t {
		R0,
		R90,
		R180,
		R270,
		// Random rotation. Useful for rendering things like grass/decorative gameworld elements.
		R_RAND,
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

		// How is this block rotated?
		BlockRotation Rotation;
		
		/*
			Information used by the terrain generator
		*/

		// Is this block part of the terrain? (if not, probably leaves, plants, structure, etc)
		bool TerrainElement : 1;

		// Is this block a resource block?
		bool Resource : 1;


	};

}
#endif // !BLOCK_H
