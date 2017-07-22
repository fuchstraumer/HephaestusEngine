#include "stdafx.h"
#include "Block.h"

namespace objects {

	BlockTypeDescription::BlockTypeDescription(uint16_t block_type){
		switch (block_type) {
			case BlockTypes::AIR:{
				DrawMode = BlockDrawMode::AIR;
				Solid = false;
				BuildableOn = false;
				Invulnerable = false;
				TerrainElement = false;
				Resource = false;
				Selectable = false;
				PropagateLight = true;
				LightEmissionIntensity = 0;
				ParamDataInfo = ParameterData::NONE;
			}
			case BlockTypes::BEDROCK: {
				DrawMode = BlockDrawMode::NORMAL;
				Solid = true;
				BuildableOn = true;
				Invulnerable = true;
				TerrainElement = false;
				Resource = false;
				Selectable = false;
				PropagateLight = false;
				LightEmissionIntensity = 0;
				ParamDataInfo = ParameterData::NONE;
			}
			default:{
				DrawMode = BlockDrawMode::NORMAL;
				Solid = true;
				BuildableOn = true;
				Invulnerable = false;
				TerrainElement = true;
				Resource = false;
				Selectable = true;
				PropagateLight = false;
				LightEmissionIntensity = 0;
				ParamDataInfo = ParameterData::NONE;
			}
		}
	}

	Block::Block(uint8_t block_type, uint8_t _param, BlockRotation rot) : type(block_type), parameters(_param), rotation(rot) {}

	Block::Block(const BlockTypes & block_type, const uint8_t & parameter, const BlockRotation & rot) : type(static_cast<uint8_t>(block_type)), parameters(parameter), rotation(rot) {}

	Block::Block(const Block& other) : type(other.type), parameters(other.parameters), rotation(other.rotation) {}

	Block::Block(Block&& other) noexcept : type(std::move(other.type)), parameters(std::move(other.parameters)), rotation(std::move(other.rotation)) {}

	Block & Block::operator=(const Block & other){
		type = other.type;
		parameters = other.parameters;
		rotation = other.rotation;
		return *this;
	}

	Block& Block::operator=(Block && other) noexcept{
		type = std::move(other.type);
		parameters = std::move(other.parameters);
		rotation = std::move(other.rotation);
		return *this;
	}

	Block::operator uint16_t() const noexcept {
		return static_cast<uint8_t>(type);
	}

	bool Block::Active() const noexcept {
		return type != static_cast<uint8_t>(BlockTypes::AIR);
	}

	bool Block::operator==(const Block & other) const{
		return (type == other.type) && (parameters == other.parameters) && (rotation == other.rotation);
	}

	uint8_t Block::GetType() const{
		return type;
	}

	void Block::SetType(const uint8_t & new_data){
		type = new_data;
	}

	void Block::SetType(uint8_t&& new_data){
		type = std::move(new_data);
	}

	uint8_t Block::GetParam() const{
		return parameters;
	}

	void Block::SetParam(const uint8_t& new_param) {
		parameters = new_param;
	}

	void Block::SetParam(uint8_t && new_param){
		parameters = std::move(new_param);
	}

	BlockRotation Block::GetRotation() const{
		return rotation;
	}

	void Block::SetRotation(const BlockRotation & new_rot){
		rotation = new_rot;
	}

	void Block::SetRotation(BlockRotation && new_rot){
		rotation = std::move(new_rot);
	}

	bool Block::Opaque() const{
		return (type != static_cast<uint8_t>(BlockTypes::AIR) && type != static_cast<uint8_t>(BlockTypes::LEAVES) && type != static_cast<uint8_t>(BlockTypes::GLASS));
	}

}