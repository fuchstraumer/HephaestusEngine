#include "stdafx.h"
#include "engine/scenes/MeshingTest.hpp"
#include "BaseScene.hpp"
namespace meshing_test {

	using namespace vulpes;

	MeshingScene::MeshingScene() : BaseScene(3, 1440, 900) {

		SetupRenderpass(Instance::VulpesInstanceConfig.MSAA_SampleCount);
		Instance::VulpesInstanceConfig.MovementSpeed = 0.05f;
		
		createChunkManager();
		SetupFramebuffers();
		

		secondaryBuffers.resize(graphicsPool->size());
		render_distance = static_cast<int>(chunkManager->GetRenderDistance());
	}

	MeshingScene::~MeshingScene() {
		chunkManager.reset();
		msaa.reset();
		gui.reset();
	}

	void MeshingScene::WindowResized() {
		chunkManager.reset();
		gui.reset();
	}

	void MeshingScene::RecreateObjects() {
		createChunkManager();
		createGUI();
	}

	void MeshingScene::RecordCommands() {

		chunkManager->Update(BaseScene::CameraPosition());

		static VkCommandBufferInheritanceInfo inherit_info = vk_command_buffer_inheritance_info_base;
		inherit_info.renderPass = renderPass->vkHandle();
		inherit_info.subpass = 0;

		gui->NewFrame(instance.get(), true);
		DrawGUI();
		chunkManager->SetLightPos(lightPos);
		chunkManager->SetLightColor(lightColor);
		chunkManager->SetRenderDistance(render_distance);

		for (uint32_t i = 0; i < graphicsPool->size(); ++i) {

			if (!secondaryBuffers[i].empty()) {
				secondaryBuffers[i].clear();
			}

			VkCommandBufferBeginInfo begin_info = vk_command_buffer_begin_info_base;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			begin_info.pInheritanceInfo = nullptr;

			VkResult err = vkBeginCommandBuffer(graphicsPool->GetCmdBuffer(i), &begin_info);
			VkAssert(err);

			renderPass->UpdateBeginInfo(framebuffers[i]);
			inherit_info.framebuffer = framebuffers[i];

			VkViewport viewport = vk_default_viewport;
			viewport.width = static_cast<float>(swapchain->Extent.width);
			viewport.height = static_cast<float>(swapchain->Extent.height);

			VkRect2D scissor = vk_default_viewport_scissor;
			scissor.extent.width = swapchain->Extent.width;
			scissor.extent.height = swapchain->Extent.height;

			vkCmdBeginRenderPass(graphicsPool->GetCmdBuffer(i), &renderPass->BeginInfo(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			inherit_info.framebuffer = framebuffers[i];
			begin_info.pInheritanceInfo = &inherit_info;

			VkCommandBuffer& terrain_buffer = secondaryPool->GetCmdBuffer(i * (swapchain->ImageCount));
			VkCommandBuffer& gui_buffer = secondaryPool->GetCmdBuffer(1 + (i * (swapchain->ImageCount)));

			renderGUI(gui_buffer, begin_info, i);
			secondaryBuffers[i].push_back(std::move(gui_buffer));

			chunkManager->Render(terrain_buffer, begin_info, GetViewMatrix(), CameraPosition(), viewport, scissor);
			secondaryBuffers[i].push_back(std::move(terrain_buffer));

			vkCmdExecuteCommands(graphicsPool->GetCmdBuffer(i), static_cast<uint32_t>(secondaryBuffers[i].size()), secondaryBuffers[i].data());

			vkCmdEndRenderPass(graphicsPool->GetCmdBuffer(i));

			err = vkEndCommandBuffer(graphicsPool->GetCmdBuffer(i));

			VkAssert(err);

		}
	}

	void MeshingScene::createChunkManager() noexcept {
		chunkManager = std::make_unique<objects::ChunkManager>(device.get(), 4);
		chunkManager->Init(glm::vec3(0.0f), 2);
		chunkManager->CreatePipeline(renderPass->vkHandle(), swapchain.get(), BaseScene::GetProjectionMatrix());
	}

	void MeshingScene::createGUI() noexcept {
		gui = std::make_unique<imguiWrapper>();
		gui->Init(device.get(), renderPass->vkHandle());
		gui->UploadTextureData(transferPool.get());
	}

	void MeshingScene::createRenderpassBeginInfo() {
		// Clear color value, clear depth value
		const static std::vector<VkClearValue> clear_values{
			VkClearValue{ 99.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f },
			VkClearValue{ 99.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f },
			VkClearValue{ 1.0f, 0 }
		};

		renderPass->SetupBeginInfo(clear_values, VkExtent2D{ swapchain->Extent.width, swapchain->Extent.height });
	}

	void MeshingScene::endFrame(const size_t& idx) {
		vkResetFences(device->vkHandle(), 1, &presentFences[idx]);
	}
}