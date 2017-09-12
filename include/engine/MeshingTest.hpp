#pragma once
#ifndef HEPH_ENGINE_MESHING_TEST_H
#define HEPH_ENGINE_MESHING_TEST_H

#include "stdafx.h"
#include "objects\ChunkManager.hpp"
#include "BaseScene.hpp"
#include "GUI_Elements.hpp"

namespace meshing_test {

	using namespace vulpes;
	using namespace objects;

	class MeshingScene : public BaseScene {
	public:

		MeshingScene() : BaseScene(3, 1440, 900) {

			SetupRenderpass(Instance::VulpesInstanceConfig.MSAA_SampleCount);
			Instance::VulpesInstanceConfig.MovementSpeed = 0.05f;
			chunkManager = std::make_unique<ChunkManager>(device.get(), 8);
			chunkManager->Init(glm::vec3(0.0f), 2);
			chunkManager->CreatePipeline(renderPass->vkHandle(), swapchain.get(), GetProjectionMatrix());
			render_distance = static_cast<int>(chunkManager->GetRenderDistance());
			SetupFramebuffers();

			auto gui_cache = std::make_shared<PipelineCache>(device.get(), static_cast<uint16_t>(typeid(imguiWrapper).hash_code()));
			gui = std::make_unique<imguiWrapper>();
			gui->Init(device.get(), renderPass->vkHandle());
			gui->UploadTextureData(transferPool.get());

			secondaryBuffers.resize(graphicsPool->size());
		}

		~MeshingScene() {
			chunkManager.reset();
			msaa.reset();
			gui.reset();
		}

		virtual void WindowResized() override {
			chunkManager.reset();
			gui.reset();
		}

		virtual void RecreateObjects() override {
			chunkManager = std::make_unique<ChunkManager>(device.get(), 4);
			chunkManager->Init(glm::vec3(0.0f), 2);
			chunkManager->CreatePipeline(renderPass->vkHandle(), swapchain.get(), GetProjectionMatrix());
			auto gui_cache = std::make_shared<PipelineCache>(device.get(), static_cast<uint16_t>(typeid(imguiWrapper).hash_code()));
			gui = std::make_unique<imguiWrapper>();
			gui->Init(device.get(), renderPass->vkHandle());
			gui->UploadTextureData(transferPool.get());
		}

		virtual void RecordCommands() override {
			// Clear color value, clear depth value
			static std::array<VkClearValue, 3> clear_values;
			clear_values[0].color = { 99.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f };
			clear_values[1].color = { 99.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f };
			clear_values[2].depthStencil = { 1.0f, 0 };

			// Given at each frame in framebuffer to describe layout of framebuffer
			static VkRenderPassBeginInfo renderpass_begin{
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				nullptr,
				renderPass->vkHandle(),
				VK_NULL_HANDLE, // update this every frame
				VkRect2D{ VkOffset2D{ 0, 0 }, swapchain->Extent },
				static_cast<uint32_t>(clear_values.size()),
				clear_values.data(),
			};
			renderpass_begin.renderPass = renderPass->vkHandle();

			chunkManager->Update(CameraPosition());
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

				// holds secondary buffers
				std::vector<VkCommandBuffer> buffers;

				VkCommandBufferBeginInfo begin_info = vk_command_buffer_begin_info_base;
				begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				begin_info.pInheritanceInfo = nullptr;

				VkResult err = vkBeginCommandBuffer(graphicsPool->GetCmdBuffer(i), &begin_info);
				VkAssert(err);

				if (device->MarkersEnabled) {
					std::string region_name = std::string("Main render loop, Swapchain image ") + std::to_string(i);
					device->vkCmdBeginDebugMarkerRegion(graphicsPool->GetCmdBuffer(i), region_name.c_str(), glm::vec4(0.0f, 0.2f, 0.8f, 1.0f));
				}

				renderpass_begin.framebuffer = framebuffers[i];
				renderpass_begin.renderArea.extent = swapchain->Extent;
				inherit_info.framebuffer = framebuffers[i];

				static VkDeviceSize offsets[1]{ 0 };

				VkViewport viewport = vk_default_viewport;
				viewport.width = static_cast<float>(swapchain->Extent.width);
				viewport.height = static_cast<float>(swapchain->Extent.height);

				VkRect2D scissor = vk_default_viewport_scissor;
				scissor.extent.width = swapchain->Extent.width;
				scissor.extent.height = swapchain->Extent.height;

				vkCmdBeginRenderPass(graphicsPool->GetCmdBuffer(i), &renderpass_begin, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

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

				if (device->MarkersEnabled) {
					device->vkCmdEndDebugMarkerRegion(graphicsPool->GetCmdBuffer(i));
				}

				err = vkEndCommandBuffer(graphicsPool->GetCmdBuffer(i));

				VkAssert(err);
				buffers.clear();
				buffers.shrink_to_fit();

			}

		}

	private:

		virtual void endFrame(const size_t& idx) override {
			vkResetFences(device->vkHandle(), 1, &presentFences[idx]);
		}

		std::unique_ptr<ChunkManager> chunkManager;
		std::vector<std::vector<VkCommandBuffer>> secondaryBuffers;
	};

}

#endif // !HEPH_ENGINE_MESHING_TEST_H
