#pragma once
#ifndef BLOCK_AREA_H
#define BLOCK_AREA_H
#include "..\Block.h"

/*
	
	Class - BlockArea

	From: https://github.com/minetest/minetest/blob/master/src/voxel.h

	Used for fast fetching of block data, and quick operations on large
	amounts of blocks at once.

*/
namespace objects {

	namespace util {

		// Declaring classes needed for function definitions (mainly for the lighting algorithms).
		class Region;
		class Chunk;

		class BlockArea {
		public:

			// Sets Min/Max to create zero area..
			BlockArea();

			// Sets Min/Max to provided values.
			BlockArea(const glm::ivec3& min, const glm::ivec3& max);

			// Uses p as the center and builds an area of with radius "radius" around p
			BlockArea(const glm::ivec3& p, const int& radius);

			// Grabs the singular block at p
			BlockArea(const glm::ivec3& p);

			// Unary operators for shifting (for +/-) and scaling (for */"/") operations

			BlockArea& operator+=(const glm::ivec3& p);

			BlockArea& operator-=(const glm::ivec3& p);

			BlockArea& operator*=(const glm::ivec3& p);

			BlockArea& operator/=(const glm::ivec3& p);

			BlockArea& operator+=(const float& f);

			BlockArea& operator-=(const float& f);

			BlockArea& operator*=(const float& f);

			BlockArea& operator/=(const float& f);

			// Comparison operator
			bool operator==(const BlockArea& other) const;

			// Merges this area with the other supplied area.
			void Merge(const BlockArea& other);

			// Adds a point - but expands to incorporate it
			void AddPoint(const glm::ivec3& p);

			// Adds padding amount p to all sides.
			void AddPadding(const size_t& p);

			// Returns size in XYZ
			glm::ivec3 GetSize() const;

			// Returns whether or not this area is empty
			bool Empty() const;

			// Returns volume in quantity of blocks
			size_t GetVolume() const;

			// Whether or not this area contains the area "other"
			bool Contains(const BlockArea& other) const;

			// Whether or not this area contains the point p
			bool Contains(const glm::ivec3& p) const;

			// Returns 0-6 overlapping areas that can be added 
			// to "area" to make up this area ("area" = copy of this)
			void Diff(const BlockArea& area, std::vector<BlockArea>& result);

			// Translates from position to linear index into container
			size_t GetIndex(const glm::ivec3& p) const;

			// Translates index in the X direction
			size_t TranslateIndexX(const glm::ivec3& extent, size_t idx, size_t amt) const;

			// Translates index in the Y direction
			size_t TranslateIndexY(const glm::ivec3& extent, size_t idx, size_t amt) const;

			// Translates index in the Z direction
			size_t TranslateIndexZ(const glm::ivec3& extent, size_t idx, size_t amt) const;

			// Translates index in XYZ using point p as translation amount
			size_t TranslateIndex(const glm::ivec3& extent, size_t idx, const glm::ivec3& amt);

			// Min/Max edges.
			glm::ivec3 Min;
			glm::ivec3 Max;

		};

		/*

			Class - BlockManipulator.

			Manipulates large areas of blocks, making for an efficient method
			of performing operations on large amounts of blocks at once.

			From MineTests VoxelManipulator class:
			https://github.com/minetest/minetest/blob/master/src/voxel.h

		*/


		class BlockManipulator {
		public:

			BlockManipulator();

			// Slow, shouldn't really be used.
			Block GetBlock(const glm::ivec3& p) const;

			// Does the block at point P exist?
			bool Exists(const glm::ivec3& p) const;

			// Get mutable reference to a block
			Block& GetBlockRef(const glm::ivec3& p);

			// Set block at point to be equal to "block"
			void SetBlock(const glm::ivec3& p, const Block& block);

			// Clears flag
			void ClearFlag(uint8_t flag);

			// Adds area to active area
			void AddArea(const BlockArea& area);

			// Copy data (blocks from 'src') from area given by block area "src_area",
			// from position "from" to position "to" with dimensions "dims"
			void CopyFrom(const std::vector<Block>& src, const BlockArea& src_area, const glm::ivec3& from,
				const glm::ivec3& to, const glm::ivec3& dims);

			// Copy data to "dest" and destiniation area "dest_area", at position "from" with destination "dest_pos",
			// with dimensions "dims"
			void CopyTo(std::vector<Block>& dest, const BlockArea& dest_area, const glm::ivec3& dest_pos,
				const glm::ivec3& from, const glm::ivec3& dims);

			// Area actively being manipulated. Stored in data.

			BlockArea Area;

		private:

			// Block data container.
			std::vector<Block> data;

			// Flags of all blocks in data (i.e this flag applied to all blocks in Area)
			std::vector<uint8_t> dataFlags;
		};


		/*

			Following are algorithms that operate on BlockAreas.

			TODO: Investigate feasibility of using unordered containers here.

			From: https://github.com/minetest/minetest/blob/master/src/voxelalgorithms.h
		*/

		// Propagate light level. bool day is true if daytime, false if night time

		static void SpreadLight(bool day, glm::ivec3& p);

		static void SpreadLight(bool day, std::set<glm::ivec3>& nodes);

		// Unpropagate light level.

		static void UnspreadLight(bool day, glm::ivec3& p, uint8_t old_light_level, std::set<glm::ivec3>& light_sources);

		static void UnspreadLight(bool day, std::map<glm::ivec3, uint8_t>& from_nodes);

		// Set light level in block manipulator "manip" and area "area" to be level "light".
		static void SetLight(BlockManipulator& manip, BlockArea& area, uint8_t light);

		// Clears light sources, but collects them together into "light_sources"
		static void ClearAndCollectLightSrcs(BlockManipulator& manip, BlockArea& area, bool day, std::set<glm::ivec3>& light_sources,
			std::map<glm::ivec3, uint8_t>& unlight_from);

		// Propagate sunlight in area given by BlockManipulator and BlockArea. Returns true for success.
		// active_sunlight tells us whether or not we should assume that a non-existent node at the highest Y-level of our currently
		// active area generates sunlight or not (kinda i.e assume a "hole" in the ceiling is a skylight, I guess)
		static bool PropagateSunlight(BlockManipulator& manip, BlockArea& area, bool active_sunlight, std::set<glm::ivec3>& light_sources);

		// Updates lighting in the given region. All new nodes in the region must have zero light level.
		// old_blocks contains the blocks (and the positions of said blocks) that were replaced by new blocks.
		// modified_chunks specifies the chunks to be updated.
		void UpdateLighting(Region& region, std::vector<std::pair<glm::ivec3, Block>>& old_blocks, std::map<glm::ivec3, Chunk>& modified_chunks);
	}

}
#endif // !BLOCK_AREA_H
