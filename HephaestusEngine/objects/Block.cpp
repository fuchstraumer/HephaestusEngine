#include "stdafx.h"
#include "Block.h"

namespace objects {

	Block::Block() : type(nullptr) {}

	Block::Block(blockType _type) {
		if (_type != blockTypes::AIR) {
			data[1] = true;
			type = new blockType;
			*type = _type;
		}
		else {
			data.reset();
		}
	}

	Block::~Block() {
		delete type;
	}

	Block::Block(const Block & other) {
		// Copy data.
		data = other.data;
		// Check type of other and either copy value
		// appropriately, or set type = nullptr
		if (other.type != nullptr) {
			type = new blockType;
			*type = *other.type;
		}
		else {
			type = nullptr;
		}
	}

	Block& Block::operator=(const Block & other) {
		// Copy data
		data = other.data;
		// Check existence of other's type and 
		// continue appropriately.
		if (other.type != nullptr) {
			type = new blockType;
			*type = *other.type;
		}
		else {
			type = nullptr;
		}
		return *this;
	}

	// Just move data from other.
	Block::Block(Block && other) : data(std::move(other.data)), type(std::move(other.type)) {}

	Block& Block::operator=(Block && other) {
		// Move data
		data = std::move(other.data);
		// Move type
		type = std::move(other.type);
		return *this;
	}

	void Block::SetActive(bool active) {
		data[0] = active;
	}

	void Block::SetOpaque(bool opaque) {
		data[1] = opaque;
	}

	void Block::SetPhysics(bool physics) {
		data[2] = physics;
	}

	void Block::Reset() {
		data.reset();
		delete type;
		type = nullptr;
	}

	void Block::SetType(blockType _type) {
		if (type != nullptr) {
			*type = _type;
		}
		else {
			type = new blockType;
			*type = _type;
		}
	}

	const bool Block::Active() const {
		return data[0];
	}

	const bool Block::Opaque() const {
		return data[1];
	}

	const bool Block::Physics() const {
		return data[2];
	}

	// If type is set, return it, otherwise return value "air"
	const blockType& Block::GetType() const {
		// TODO: Why do I get a returning address of local var/temp warning?
		return (type != nullptr) ? blockType(*type) : blockType(blockTypes::AIR);
	}

}