#pragma once
#ifndef INVENTORY_H
#define INVENTORY_H
#include "stdafx.h"
#include "common/CommonUtil.h"

/*
	
	INVENTORY_H

	Defines a couple of classes and structs for managing the inventory
	of a single player:

	struct - ItemStack

	Represents a homogenuous collection of items (of a uniform type). 

	class - InventoryData

	Collection of item stacks that makes up most of actual data in a player inventory.
	This can be saved to and loaded from a file. This object is effectively a 2D array
	of ItemStacks.

	class - Inventory

	Represents the active in-world inventory attached to a player. Contains ItemData. 

*/

namespace objects::util {

	/*
		struct - ItemStack
	*/

	struct ItemStack {

		// Ctor - empty
		ItemStack();

		// Ctor - type of item and quantity
		ItemStack(uint16_t item_type, size_t amt = 1);

		// Is the ItemStack empty?
		bool empty() const;

		// Empty/clear the stack
		bool Clear() const;

		// Add quantity n to the stack
		void Add(size_t n);

		// Max size of a stack of item_type
		static size_t MaxStackSize();

		// Free space in this stack
		size_t FreeSpace() const;

		// Add another item stack. Adds whole stack if possible, returns remainder if not able to add whole
		// stack, returns empty if cna't add at all
		ItemStack AddStack(const ItemStack& other);

		// Check if stack "other" can be merged into this stack
		bool StackFits(const ItemStack& other);

		// Removes quantity n items from this stack, returning it as a new stack
		ItemStack TakeItems(size_t n);

		// Similar to above, but doesn't modify this object.
		ItemStack Peek(size_t n) const;


		// Name/type of items in this object
		uint16_t Type;
		// Used with GUI and for saving to file.
		std::string Name; 
		// Number of items of type "Type" in this stack.
		size_t Quantity;
		// Wear level for items like tools that can be "worn"
		uint16_t Wear; 

	};

	/*
	
		struct - InventoryData
	
	*/
	class InventoryData {
	public:

		// Creates instance of inventory data with name "name" and size "sz"
		InventoryData(std::string name, size_t sz);

		// Defaulted dtor
		~InventoryData() = default;

		// Copy and move stuff

		InventoryData(const InventoryData& other);
		InventoryData& operator=(const InventoryData& other);

		InventoryData(InventoryData&& other);
		InventoryData& operator=(InventoryData&& other);

		// Boolean operators
		bool operator==(const InventoryData& other) const;
		bool operator!=(const InventoryData& other) const;

		// Clear items in this object
		void Clear();

		// Set inventory size in X
		void SetWidth(size_t width);

		// Set inventory size in Y
		void SetHeight(size_t height);

	};

}

#endif // !INVENTORY_H