#pragma once
#ifndef HEPH_ENGINE_MESHING_TEST_H
#define HEPH_ENGINE_MESHING_TEST_H

#include "stdafx.h"
#include "engine/objects/ChunkManager.hpp"
#include "BaseScene.hpp"
#include "GUI_Elements.hpp"

namespace meshing_test {


	class MeshingScene : public vulpes::BaseScene {
	public:

		MeshingScene();
		~MeshingScene();

		virtual void WindowResized() override;
		virtual void RecreateObjects() override;
		virtual void RecordCommands() override;

	private:

		void createChunkManager() noexcept;
		void createGUI() noexcept;
		void createRenderpassBeginInfo();
		virtual void endFrame(const size_t& idx) override;

		std::unique_ptr<objects::ChunkManager> chunkManager;
		std::vector<std::vector<VkCommandBuffer>> secondaryBuffers;
	};

}

#endif // !HEPH_ENGINE_MESHING_TEST_H
