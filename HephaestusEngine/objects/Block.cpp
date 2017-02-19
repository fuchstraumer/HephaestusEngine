#include "stdafx.h"
#include "Block.h"

namespace objects {

	Block::Block() : type(blockTypes::AIR) {}

	Block::Block(blockType _type) {
		if (_type != blockTypes::AIR) {
			active = true;
			type =  _type;
			if (type != blockTypes::GLASS && type != blockTypes::GRASS && type != blockTypes::TALL_GRASS) {
				opaque = false;
			}
			physics = false;
		}
		else {
			type = _type;
			active = false;
			opaque = true;
			physics = false;
		}
	}

	Block::Block(const Block & other) : active(other.active), opaque(other.opaque), physics(other.physics), type(other.type) {}

	Block& Block::operator=(const Block & other) {
		active = other.active;
		opaque = other.opaque;
		physics = other.physics;
		type = other.type;
		return *this;
	}

	// Just move data from other.
	Block::Block(Block && other) : active(other.active), opaque(other.opaque), physics(other.physics), type(std::move(other.type)) {}

	Block& Block::operator=(Block && other) {
		active = other.active;
		opaque = other.opaque;
		physics = other.physics;
		type = std::move(other.type);
		return *this;
	}

	void Block::SetActive(bool _active) {
		active = _active;
	}

	void Block::SetOpaque(bool _opaque) {
		opaque = _opaque;
	}

	void Block::SetPhysics(bool _physics) {
		physics = _physics;
	}

	void Block::Reset() {
		active = false;
		opaque = false;
		physics = false;
		type = blockTypes::AIR;
	}

	void Block::SetType(blockType _type) {
		type = _type;
	}

	const bool Block::Active() const {
		return active;
	}

	const bool Block::Opaque() const {
		return opaque;
	}

	const bool Block::Physics() const {
		return physics;
	}

	// If type is set, return it, otherwise return value "air"
	const blockType& Block::GetType() const {
		// TODO: Why do I get a returning address of local var/temp warning?
		return type;
	}

}