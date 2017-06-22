#include "stdafx.h"
#include "ChunkManager.h"

namespace objects {

	using namespace vulpes;

	ChunkManager::ChunkManager(const size_t & init_view_radius) : renderRadius(init_view_radius) {

		static const std::array<VkDescriptorPoolSize, 1> pools{
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
		};

		VkDescriptorPoolCreateInfo pool_cr_info = vk_descriptor_pool_create_info_base;
		pool_cr_info.maxSets = 1;
		pool_cr_info.poolSizeCount = static_cast<uint32_t>(pools.size());
		pool_cr_info.pPoolSizes = pools.data();

		VkResult result = vkCreateDescriptorPool(device->vkHandle(), &pool_cr_info, nullptr, &descriptorPool);
		VkAssert(result);

		static std::array<VkDescriptorSetLayoutBinding, 1> bindings{
			VkDescriptorSetLayoutBinding{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
		};

		VkDescriptorSetLayoutCreateInfo set_layout_info = vk_descriptor_set_layout_create_info_base;
		set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
		set_layout_info.pBindings = bindings.data();

		result = vkCreateDescriptorSetLayout(device->vkHandle(), &set_layout_info, nullptr, &descriptorSetLayout);
		VkAssert(result);

		static const std::array<VkPushConstantRange, 2> push_constants{
			VkPushConstantRange{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(uboData) },
			VkPushConstantRange{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData), sizeof(glm::vec4) }, // camera position.
		};

		VkPipelineLayoutCreateInfo pipeline_layout_info = vk_pipeline_layout_create_info_base;
		pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_constants.size());
		pipeline_layout_info.pPushConstantRanges = push_constants.data();
		pipeline_layout_info.pSetLayouts = &descriptorSetLayout;
		pipeline_layout_info.setLayoutCount = 1;

		result = vkCreatePipelineLayout(device->vkHandle(), &pipeline_layout_info, nullptr, &pipelineLayout);
		VkAssert(result);

		VkDescriptorSetAllocateInfo alloc_info = vk_descriptor_set_alloc_info_base;
		alloc_info.descriptorPool = descriptorPool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &descriptorSetLayout;

		result = vkAllocateDescriptorSets(device->vkHandle(), &alloc_info, &descriptorSet);
		VkAssert(result);

		vert = std::make_unique<ShaderModule>(device, "rsrc/shaders/block/block.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		frag = std::make_unique<ShaderModule>(device, "rsrc/shaders/block/block.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		cache = std::make_unique<PipelineCache>(device, static_cast<int16_t>(typeid(*this).hash_code()));
	}

	void ChunkManager::Update(const glm::vec3& update_position) {
		// For each chunk in the list of active chunks, check them for any blocks that have changed.

		// Check to see if our position has changed enough that we should be checking for chunks to load/unload

	}

	void ChunkManager::Render(VkCommandBuffer & cmd, VkCommandBufferBeginInfo & begin_info, const glm::mat4 & view, const glm::vec3 & camera_pos, const VkViewport & viewport, const VkRect2D & scissor) {
		uboData.view = view;
		VkResult result = vkBeginCommandBuffer(cmd, &begin_info);
		VkAssert(result);

		if (device->MarkersEnabled) {
			device->vkCmdBeginDebugMarkerRegion(cmd, "Draw chunks", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}

		if (!renderChunks.empty()) {
			vkCmdSetViewport(cmd, 0, 1, &viewport);
			vkCmdSetScissor(cmd, 0, 1, &scissor);
			vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vkHandle());
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
			glm::vec4 camera_push = glm::vec4(camera_pos.x, camera_pos.y, camera_pos.z, 0.0f); // use vec4 for sake of alignment.
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData), sizeof(glm::vec4), glm::value_ptr(camera_push));
			// push view + projection.
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), sizeof(glm::mat4) * 2, &uboData.view);
			for (auto iter = renderChunks.cbegin(); iter != renderChunks.end(); ++iter) {
				Chunk* curr = *iter;
				vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), glm::value_ptr(uboData.model));
				curr->mesh->render(cmd);
			}
		}

		result = vkEndCommandBuffer(cmd);
		VkAssert(result);
	}



}