#pragma once
#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H
#include "stdafx.h"
#include "Chunk.h"
#include "resource/PipelineCache.h"
#include "render/GraphicsPipeline.h"
#include "resource/ShaderModule.h"
#include "command/TransferPool.h"
#include "resource/Texture.h"

namespace objects {

	

	// Type of events/flags that require us to update an individual chunk
	enum class EditEventType {
		ADD_BLOCK, // New block added
		REMOVE_BLOCK, // Block removed
		SWAP_BLOCK, // Blocks swapped
		EDIT_BLOCK, // Block data edited
		ADD_LIGHT_BLOCK, // Light block added. Update lighting.
		LIGHT_UPDATE, // Just update lighting.
	};

	// Struct used to encapsulate an event.
	struct EditEvent {
		// Type of this event.
		EditEventType Type;
		// Position of this event
		glm::vec3 Position;
		// Block that was edited
		Block EditedBlock;
		// Positions of other affected blocks
		std::set<glm::vec3> ModifiedBlocks;
		// TODO: Some way of propagating change/update status between server/client.
	};

	// Used in lighting update.
	struct LightNode {
		short Idx; // Index into container.
		std::shared_ptr<Chunk> Parent;
		LightNode(const glm::ivec3& p, Chunk* parent) : Parent(std::shared_ptr<Chunk>(parent)) {
			Idx = p.y * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE + p.x * Chunk::CHUNK_SIZE + p.z;
		}
	};

	class ChunkManager {
	public:

		ChunkManager(const size_t& init_view_radius);

		~ChunkManager();

		void CreatePipeline(const VkRenderPass& renderpass, const vulpes::Swapchain* swapchain, const glm::mat4& projection);

		void CreateChunk(const glm::ivec2& grid_position);

		// Initialize the chunk manager by starting at an initial position and using the input
		// view distance (given in terms of a radius of chunks to render)
		void Init(const glm::vec3 & initial_position, const unsigned int& view_distance);

		void Update(const glm::vec3& update_position);

		// Renders chunks in the 
		void Render(VkCommandBuffer& cmd, VkCommandBufferBeginInfo& begin_info, const glm::mat4& view, const glm::vec3& camera_pos, const VkViewport& viewport, const VkRect2D& scissor);

		// Cleans up inactive chunks in "pruneChunks" by compressing and then saving their data.
		void Prune();

	private:

		void updateLighting();
		void transferChunksToDevice();

		void transferChunkToDevice(std::shared_ptr<Chunk>& chunk_to_transfer) const;

		void createDescriptors();
		void createPipelineLayout();
		void allocateDescriptors();
		void setupPipelineInfo();

		struct chunkUBO {
			glm::mat4 model, view, projection;
		} uboData;

		// Radius, in chunks, to render
		size_t renderRadius;

		// Main container of chunk data, map allows for searching based on the chunks position.
		std::unordered_map<glm::ivec2, std::shared_ptr<Chunk>> chunkMap;

		std::forward_list<std::shared_ptr<Chunk>> transferChunks;
		std::set<std::shared_ptr<Chunk>> renderChunks;

		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkGraphicsPipelineCreateInfo pipelineCreateInfo;

		std::unique_ptr<vulpes::PipelineCache> cache;
		std::unique_ptr<vulpes::ShaderModule> vert, frag;
		//std::unique_ptr<vulpes::TextureArray> blockTextures;
		std::unique_ptr<vulpes::TransferPool> transferPool;
		std::unique_ptr<vulpes::GraphicsPipeline> pipeline;
		vulpes::GraphicsPipelineInfo pipelineInfo;
		const vulpes::Device* device;

	};
}
#endif // !CHUNK_MANAGER_H
