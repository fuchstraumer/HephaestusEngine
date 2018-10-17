#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include "stdafx.h"

namespace objects {

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
