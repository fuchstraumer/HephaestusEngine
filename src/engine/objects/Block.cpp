#include "stdafx.h"
#include "engine/objects/Block.hpp"

namespace objects {

	Block::Block(uint8_t block_type, uint8_t _param, BlockRotation rot) : type(block_type) {}

	Block::Block(const BlockTypes & block_type, const uint8_t & parameter, const BlockRotation & rot) : type(static_cast<uint8_t>(block_type)) {}

	Block::Block(const Block& other) : type(other.type) {}

	Block::Block(Block&& other) noexcept : type(std::move(other.type)) {}

	Block & Block::operator=(const Block & other){
		type = other.type;
		return *this;
	}

	Block& Block::operator=(Block && other) noexcept{
		type = std::move(other.type);
		return *this;
	}

	Block::operator uint16_t() const noexcept {
		return static_cast<uint8_t>(type);
	}

	bool Block::Active() const noexcept {
		return type != static_cast<uint8_t>(BlockTypes::AIR);
	}

	bool Block::operator==(const Block & other) const{
		return (type == other.type);
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


	bool Block::Opaque() const{
		return (type != static_cast<uint8_t>(BlockTypes::AIR) && type != static_cast<uint8_t>(BlockTypes::LEAVES) && type != static_cast<uint8_t>(BlockTypes::GLASS));
	}

}