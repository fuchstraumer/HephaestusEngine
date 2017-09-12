#include "stdafx.h"
#include "engine/objects/ChunkManager.hpp"
#include "core/Instance.hpp"
namespace objects {

	using namespace vulpes;

	struct area_t {
		glm::ivec2 min, max;
	};

	ChunkManager::ChunkManager(const vulpes::Device* parent_device, const size_t & init_view_radius) : renderRadius(init_view_radius), device(parent_device), pipelineCreateInfo(vulpes::vk_graphics_pipeline_create_info_base) {

		createDescriptors();
		createPipelineLayout();
		allocateDescriptors();

		vert = std::make_unique<ShaderModule>(device, "./rsrc/shaders/terrain/block.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		frag = std::make_unique<ShaderModule>(device, "./rsrc/shaders/terrain/block.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
		cache = std::make_unique<PipelineCache>(device, static_cast<int16_t>(typeid(*this).hash_code()));
		transferPool = std::make_unique<TransferPool>(device);
		
		fragmentUBO.lightColor = glm::vec4(225.0f / 255.0f, 229.0f / 255.0f, 1.0f, 1.0f);
		fragmentUBO.lightPos = glm::vec4(0.0f, 500.0f, 0.0f, 1.0f);

		blockTexture = std::make_unique<vulpes::Texture<gli::texture2d_array>>(device);
		blockTexture->CreateFromFile("./rsrc/textures/arrays/blocks.dds", VK_FORMAT_R8G8B8A8_UNORM);
		
		auto cmd = transferPool->Begin();
		blockTexture->TransferToDevice(cmd);
		transferPool->End();
		transferPool->Submit();

		updateWriteDescriptors();

	}

	ChunkManager::~ChunkManager() {
		vkFreeDescriptorSets(device->vkHandle(), descriptorPool, 1, &descriptorSet);
		vkDestroyPipelineLayout(device->vkHandle(), pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device->vkHandle(), descriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(device->vkHandle(), descriptorPool, nullptr);
		renderChunks.clear();
	}

	void ChunkManager::CreatePipeline(const VkRenderPass & renderpass, const Swapchain * swapchain, const glm::mat4 & projection) {
		
		uboData.projection = projection;

		const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{
			vert->PipelineInfo(),
			frag->PipelineInfo(),
		};

		static VkPipelineVertexInputStateCreateInfo vert_info = mesh::BlockVertices::PipelineInfo();
		static const auto binding_info = mesh::BlockVertices::BindDescr();
		static const auto attribute_info = mesh::BlockVertices::AttrDescr();
		vert_info.pVertexAttributeDescriptions = attribute_info.data();
		vert_info.pVertexBindingDescriptions = binding_info.data();

		setupPipelineInfo();
		pipelineCreateInfo.pStages = shader_stages.data();
		pipelineCreateInfo.pVertexInputState = &vert_info;
		pipelineCreateInfo.renderPass = renderpass;
		pipelineCreateInfo.layout = pipelineLayout;
		pipeline = std::make_unique<GraphicsPipeline>(device);
		pipeline->Init(pipelineCreateInfo, cache->vkHandle());

	}

	void ChunkManager::CreateChunk(const glm::ivec2 & grid_position) {
		
		auto new_chunk = std::make_shared<Chunk>(grid_position);
		new_chunk->BuildTerrain(0);
		chunkMap.insert(std::make_pair(grid_position, new_chunk));
		transferChunks.push_front(new_chunk);

	}

	void ChunkManager::Init(const glm::vec3 & initial_position, const int & view_distance) {

		area_t area;
	
		for (int i = 0; i < view_distance; ++i) {
			area.min = glm::ivec2(static_cast<int>(initial_position.x) - i, static_cast<int>(initial_position.y) - i);
			area.max = glm::ivec2(static_cast<int>(initial_position.x) + i, static_cast<int>(initial_position.y) + i);

			for (int x = area.min.x; x < area.max.x; ++x) {
				for (int y = area.min.y; y < area.max.y; ++y) {
					CreateChunk(glm::ivec2(x, y));
				}
			}
		}

	}

	void ChunkManager::SetRenderDistance(const size_t& render_distance) {
		renderRadius = render_distance;
	}

	size_t ChunkManager::GetRenderDistance() const noexcept {
		return renderRadius;
	}

	void ChunkManager::Update(const glm::vec3 & update_position) {
		if (!transferChunks.empty()) {
			transferChunksToDevice();
		}

		glm::ivec2 camera_chunk_pos = glm::ivec2(static_cast<int>(update_position.x) / CHUNK_SIZE, static_cast<int>(update_position.z) / CHUNK_SIZE);

		{
			
			area_t area;
			for (size_t i = 0; i < renderRadius; ++i) {
				area.min = camera_chunk_pos - static_cast<int>(i);
				area.max = camera_chunk_pos + static_cast<int>(i);

				for (int x = area.min.x; x < area.max.x; ++x) {
					for (int y = area.min.y; y < area.max.y; ++y) {
						if (chunkMap.count(glm::ivec2(x, y)) == 0) {
							CreateChunk(glm::ivec2(x, y));
						}
					}
				}
			}

		}

		{

			area_t area;
			area.min = camera_chunk_pos - static_cast<int>(renderRadius);
			area.max = camera_chunk_pos + static_cast<int>(renderRadius);

			auto iter = chunkMap.begin();
			while(iter != chunkMap.end()) {
				const auto& pos = iter->second->GridPosition;
				if (pos.x <= area.min.x || pos.y <= area.min.y || pos.x >= area.max.x || pos.y >= area.max.y) {
					renderChunks.erase((iter++)->second);
					chunkMap.erase(pos);
				}
				else {
					++iter;
				}
			}
		}
	}

	void ChunkManager::SetLightPos(const glm::vec3 & light_pos) {
		fragmentUBO.lightPos.xyz = light_pos;
	}

	void ChunkManager::SetLightColor(const glm::vec3 & light_color) {
		fragmentUBO.lightColor.xyz = light_color;
	}

	glm::vec3 ChunkManager::GetLightPos() const noexcept {
		return fragmentUBO.lightPos.xyz;
	}

	glm::vec3 ChunkManager::GetLightColor() const noexcept {
		return fragmentUBO.lightColor.xyz;
	}

	void ChunkManager::transferChunksToDevice() {

		std::vector<std::future<void>> mesh_launches;

		for (auto iter = transferChunks.begin(); iter != transferChunks.end(); ++iter) {
			auto& chunk_ptr = *iter;
			mesh_launches.push_back(std::async(std::launch::async, &Chunk::BuildMesh, chunk_ptr.get()));
		}

		for (auto&& fut : mesh_launches) {
			fut.get();
		}

		transferPool->Begin();

		std::forward_list<std::shared_ptr<Chunk>> transferred_chunks;

		while (!transferChunks.empty()) {
			auto curr_chunk = transferChunks.front();
			transferChunks.pop_front();
			curr_chunk->CreateMeshBuffers(device);
			transferChunkToDevice(curr_chunk);
			renderChunks.insert(curr_chunk);
			transferred_chunks.push_front(curr_chunk);
		}

		transferPool->End();
		transferPool->Submit();

	}

	void ChunkManager::transferChunkToDevice(std::shared_ptr<Chunk>& chunk_to_transfer) const {
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
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData), sizeof(glm::vec4), glm::value_ptr(fragmentUBO.lightColor));
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData) + sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(fragmentUBO.lightPos));
			glm::vec4 camera_push = glm::vec4(camera_pos.x, camera_pos.y, camera_pos.z, 0.0f); // use vec4 for sake of alignment.
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData) + (2 * sizeof(glm::vec4)), sizeof(glm::vec4), glm::value_ptr(camera_push));
			// push view + projection.
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(uboData.view));
			vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(uboData.projection));
			for (auto iter = renderChunks.cbegin(); iter != renderChunks.end(); ++iter) {
				uboData.model = (*iter)->mesh->model;
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
			VkPushConstantRange{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(uboData), sizeof(fragment_UBO) },
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

	void ChunkManager::updateWriteDescriptors() {

		const VkDescriptorImageInfo image_info{ blockTexture->Sampler(), blockTexture->View(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		const VkWriteDescriptorSet image_write_descriptor{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &image_info, nullptr, nullptr };

		vkUpdateDescriptorSets(device->vkHandle(), 1, &image_write_descriptor, 0, nullptr);

	}

	void ChunkManager::setupPipelineInfo() {

		static const std::array<VkDynamicState, 2>  dynamic_states{
			VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
		};

		pipelineInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		pipelineInfo.DynamicStateInfo.pDynamicStates = dynamic_states.data();
		pipelineInfo.MultisampleInfo.rasterizationSamples = vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount;
		pipelineInfo.MultisampleInfo.sampleShadingEnable = vulpes::Instance::VulpesInstanceConfig.EnableMSAA;
		pipelineInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		pipelineInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipelineInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pInputAssemblyState = &pipelineInfo.AssemblyInfo;
		pipelineCreateInfo.pTessellationState = nullptr;
		pipelineCreateInfo.pViewportState = &pipelineInfo.ViewportInfo;
		pipelineCreateInfo.pRasterizationState = &pipelineInfo.RasterizationInfo;
		pipelineCreateInfo.pMultisampleState = &pipelineInfo.MultisampleInfo;
		pipelineCreateInfo.pDepthStencilState = &pipelineInfo.DepthStencilInfo;
		pipelineCreateInfo.pColorBlendState = &pipelineInfo.ColorBlendInfo;
		pipelineCreateInfo.pDynamicState = &pipelineInfo.DynamicStateInfo;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.subpass = 0;

	}

}