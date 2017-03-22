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

	Block::Block(uint8_t block_type, uint8_t _param, BlockRotation rot) : type(block_type), param(_param), rotation(rot) {}

	Block::Block(const Block& other) : type(other.type), param(other.param), rotation(other.rotation) {}

	Block::Block(Block&& other) noexcept : type(std::move(other.type)), param(std::move(other.param)), rotation(std::move(other.rotation)) {}

	Block & Block::operator=(const Block & other){
		type = other.type;
		param = other.param;
		rotation = other.rotation;
		return *this;
	}

	Block& Block::operator=(Block && other) noexcept{
		type = std::move(other.type);
		param = std::move(other.param);
		rotation = std::move(other.rotation);
		return *this;
	}

	bool Block::operator==(const Block & other) const{
		return (type == other.type) && (param == other.param) && (rotation == other.rotation);
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
		return param;
	}

	void Block::SetParam(const uint8_t& new_param) {
		param = new_param;
	}

	void Block::SetParam(uint8_t && new_param){
		param = std::move(new_param);
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
		return (type != BlockTypes::AIR && type != BlockTypes::LEAVES && type != BlockTypes::GLASS);
	}

}