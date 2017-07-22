#include "stdafx.h"
#include "ChunkManager.h"

namespace objects {

	using namespace vulpes;

	ChunkManager::ChunkManager(const vulpes::Device* parent_device, const size_t & init_view_radius) : renderRadius(init_view_radius), device(parent_device) {

		createDescriptors();
		createPipelineLayout();
		allocateDescriptors();
		vert = std::make_unique<ShaderModule>(device, "rsrc/shaders/debug.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		frag = std::make_unique<ShaderModule>(device, "rsrc/shaders/debug.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
		cache = std::make_unique<PipelineCache>(device, static_cast<int16_t>(typeid(*this).hash_code()));
	
	}

	ChunkManager::~ChunkManager() {
		vkFreeDescriptorSets(device->vkHandle(), descriptorPool, 1, &descriptorSet);
		vkDestroyPipelineLayout(device->vkHandle(), pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device->vkHandle(), descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(device->vkHandle(), descriptorPool, nullptr);
	}

	void ChunkManager::CreatePipeline(const VkRenderPass & renderpass, const Swapchain * swapchain, const glm::mat4 & projection) {
		
		uboData.projection = projection;

		const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{
			vert->PipelineInfo(),
			frag->PipelineInfo(),
		};

		static const VkPipelineVertexInputStateCreateInfo vert_info = mesh::BlockVertices::PipelineInfo();
		setupPipelineInfo();
		pipelineCreateInfo.pStages = shader_stages.data();
		pipelineCreateInfo.pVertexInputState = &vert_info;
		pipelineCreateInfo.renderPass = renderpass;

		pipeline = std::make_unique<GraphicsPipeline>(device);
		pipeline->Init(pipelineCreateInfo, cache->vkHandle());

	}

	void ChunkManager::CreateChunk(const glm::ivec2 & grid_position) {
		
		auto new_chunk = std::make_shared<Chunk>(grid_position);
		std::async(std::launch::async, &Chunk::BuildTerrain, new_chunk.get(), 0);
		chunkMap.insert(std::make_pair(grid_position, new_chunk));
		transferChunks.push_front(new_chunk);

	}

	void ChunkManager::Init(const glm::vec3 & initial_position, const unsigned int & view_distance) {

		for (size_t j = 0; j < view_distance; ++j) {
			for (size_t i = 0; i < view_distance; ++i) {
				if ((i * i) + (j * j) <= (view_distance * view_distance)) {
					CreateChunk(glm::ivec2(i, j));
				}
			}
		}

	}

	void ChunkManager::Update(const glm::vec3 & update_position) {
		if (!transferChunks.empty()) {
			transferChunksToDevice();
		}
	}

	void ChunkManager::transferChunksToDevice() {
		transferPool->Begin();

		while (!transferChunks.empty()) {
			auto& curr_chunk = transferChunks.front();
			transferChunks.pop_front();
			transferChunkToDevice(curr_chunk);
			renderChunks.insert(curr_chunk);
		}

		transferPool->End();
		transferPool->Submit();
	}

	void ChunkManager::transferChunkToDevice(std::shared_ptr<Chunk>& chunk_to_transfer) const {
		chunk_to_transfer->BuildMesh();
		chunk_to_transfer->mesh->record_transfer_commands(transferPool->GetCmdBuffer(0));
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
				vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), glm::value_ptr(uboData.model));
				(*iter)->mesh->render(cmd);
			}
		}

		result = vkEndCommandBuffer(cmd);
		VkAssert(result);
	}

	void ChunkManager::createDescriptors() {

		static const std::array<VkDescriptorPoolSize, 1> pools{
			VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
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

	}

	void ChunkManager::createPipelineLayout() {
		
		static const std::array<VkPushConstantRange, 2> push_constants{
			VkPushConstantRange{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(uboData) },
			VkPushConstantRange{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData), sizeof(glm::vec4) }, // camera position.
		};

		VkPipelineLayoutCreateInfo pipeline_layout_info = vk_pipeline_layout_create_info_base;
		pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_constants.size());
		pipeline_layout_info.pPushConstantRanges = push_constants.data();
		pipeline_layout_info.pSetLayouts = &descriptorSetLayout;
		pipeline_layout_info.setLayoutCount = 1;

		VkResult result = vkCreatePipelineLayout(device->vkHandle(), &pipeline_layout_info, nullptr, &pipelineLayout);
		VkAssert(result);

	}

	void ChunkManager::allocateDescriptors() {

		VkDescriptorSetAllocateInfo alloc_info = vk_descriptor_set_alloc_info_base;
		alloc_info.descriptorPool = descriptorPool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &descriptorSetLayout;

		VkResult result = vkAllocateDescriptorSets(device->vkHandle(), &alloc_info, &descriptorSet);
		VkAssert(result);

	}

	void ChunkManager::setupPipelineInfo() {

		static const std::array<VkDynamicState, 2>  dynamic_states{
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
		};

		pipelineInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		pipelineInfo.DynamicStateInfo.pDynamicStates = dynamic_states.data();
		pipelineInfo.MultisampleInfo.rasterizationSamples = vulpes::Multisampling::SampleCount;
		
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pInputAssemblyState = &pipelineInfo.AssemblyInfo;
		pipelineCreateInfo.pTessellationState = nullptr;
		pipelineCreateInfo.pViewportState = nullptr;
		pipelineCreateInfo.pRasterizationState = &pipelineInfo.RasterizationInfo;
		pipelineCreateInfo.pMultisampleState = &pipelineInfo.MultisampleInfo;
		pipelineCreateInfo.pDepthStencilState = &pipelineInfo.DepthStencilInfo;
		pipelineCreateInfo.pColorBlendState = &pipelineInfo.ColorBlendInfo;
		pipelineCreateInfo.pDynamicState = &pipelineInfo.DynamicStateInfo;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

	}

}