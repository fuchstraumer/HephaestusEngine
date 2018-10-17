#pragma once
#ifndef HEPHEASTUS_ENGINE_BLOCK_TYPE_DESCRIPTION_HPP
#define HEPHEASTUS_ENGINE_BLOCK_TYPE_DESCRIPTION_HPP
#include <cstdint>

enum class BlockDrawMode : uint8_t {
    AIR, // An air block, i.e don't draw it at all
    NORMAL, // A solid block
    LIQUID, // Liquid block. Partially opaque.
    GLASS, // Glass block, or glass-like block.
    PLANT, // Draw this block like it was a plant
    LEAVES, // Draw this block like its a leaf block
};

// Type of data kept in the parameter data item
enum class ParameterData : uint8_t {
    NONE, // Nothing, no need to use param
    FULL, // Need full space of param.
    FACE_DIRECTION, // Used to set proper direction: important for blocks like chests
    DIRECTION, // Not like face direction: sets more granular direction. used for plants.
    MESH_OPTIONS, // Used with plant rendering, leave rendering, etc
};

/*

    Struct - BlockTypeDescription

    This struct is initialized with a BlockType value. Using this, it sets its 
    internal values appropriately upon init and can then be used for querying
    the properties of a certain blocktype.

*/
struct BlockTypeDescription {

    BlockTypeDescription(uint16_t block_type);

    /*
        Base properties.
    */

    // Whether or not the block type is solid
    bool Solid : 1;

    // Whether or not this block type can be built upon
    bool BuildableOn : 1;

    // Whether or not this object can be placed/destroyed
    bool Invulnerable : 1;

    // Whether or not this object can be highlighted in-game
    bool Selectable : 1;

    // What kind of data does this block type store in param1?
    ParameterData ParamDataInfo;

    /*
        Lighting properties.
    */

    // Should light propagate through this block?
    bool PropagateLight : 1;

    // How much light does this type of block emit (if it does)?
    uint8_t LightEmissionIntensity;

    /*
        Rendering information
    */

    // How to render this type of block
    BlockDrawMode DrawMode;
    
    /*
        Information used by the terrain generator
    */

    // Is this block part of the terrain? (if not, probably leaves, plants, structure, etc)
    bool TerrainElement : 1;

    // Is this block a resource block?
    bool Resource : 1;

};

#endif //HEPHEASTUS_ENGINE_BLOCK_TYPE_DESCRIPTION_HPP
