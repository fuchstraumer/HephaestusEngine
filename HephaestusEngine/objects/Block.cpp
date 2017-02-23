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

}