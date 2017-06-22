#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include "stdafx.h"

namespace objects {

	/*

		LIGHT_H

		Defines various methods, macros, and const "things" related to lighting. (for objects!)
		Lighting data is stored as a small part of a block object, but we also
		want to be able to easily query and interact with lighting data in a way
		that isn't solely tied to a single block object: so, much of the lighting
		stuff is defined here.

		// TODO: More detail!

		Original source:
		https://github.com/minetest/minetest/blob/master/src/light.h
		https://github.com/minetest/minetest/blob/master/src/light.cpp

	*/

	// Forward declare this, we store a weak ptr to it in LightingManager.
	class Chunk;

	struct LightNode {

		LightNode(short idx);

		short index;

	};

	class ChunkLightingManager {
	public:

		ChunkLightingManager(Chunk* parent);

		std::queue<LightNode> LightingQueue;

		void Update();
		
		void emplace(const LightNode& node);

		std::weak_ptr<Chunk> Parent;

	};

}

#endif // !LIGHT_H
