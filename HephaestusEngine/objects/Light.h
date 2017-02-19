#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include "stdafx.h"

/*
	
	LIGHT_H

	Defines various methods, macros, and const "things" related to lighting.
	Lighting data is stored as a small part of a block object, but we also
	want to be able to easily query and interact with lighting data in a way
	that isn't solely tied to a single block object: so, much of the lighting
	stuff is defined here.

	// TODO: More detail!

	Original source:
	https://github.com/minetest/minetest/blob/master/src/light.h
	https://github.com/minetest/minetest/blob/master/src/light.cpp

*/

// Light intensity data is stored using 4 bits alone, so the absolute max
// is 15. We want the sun to be the brightest though, so we cap our non-sunlight
// sources at having a max intensity of 14 and reserve 15 for the sun alone.
constexpr int MAX_LIGHT_INTENSITY = 14;
constexpr int SUNLIGHT_INTENSITY = 15;

// Used to dim a light
inline uint8_t DimLight(uint8_t light) {
	// Can't have negative intensity.
	if (light == 0) {
		return 0;
	}
	// Don't interfere with intensity limits
	else if (light >= MAX_LIGHT_INTENSITY) {
		return MAX_LIGHT_INTENSITY - 1;
	}
	// Decrease intensity by one.
	else {
		return light--;
	}
}

// Dim a light by amount "amt"
inline uint8_t DimLight(uint8_t light, uint8_t amt) {
	if (amt >= light) {
		return 0;
	}
	else {
		return light - amt;
	}
}

// Brighten light
inline uint8_t BrigtenLight(uint8_t light) {
	// Usually don't want to brighten from 0: objects at 0 are there for a reason.
	if (light == 0) {
		return 0;
	}
	else if (light == MAX_LIGHT_INTENSITY) {
		return light;
	}
	else {
		// Increase light intensity.
		return light++;
	}
}

/*
	
	The following methods are for converting between various ways of measuring light intensity.
	We construct look-up tables for going between the light values used in rendering and the 
	light values used by game logic. We also create these tables using a "gamma" value, so that
	we get a uniform/pleasing presentation of brightness.

	See: https://learnopengl.com/#!Advanced-Lighting/Gamma-Correction

	for more about Gamma

*/

static uint8_t Light_LUT[SUNLIGHT_INTENSITY];

// Use this to either init the light LUT, or update it if the user has changed the gamma parameter.
/*
	Gamma range: 1.1 <= gamma <= 3.0

	This isn't a simple/true/linear gamma encoding scheme as we are adjusting based on how the internal
	game logic handles light intensities, light propagation, and the like.

	Gamma = 1.0/1.1 = linear light table. Typical values will be 1.8-2.2 (according to minetest source)
*/
void SetLightLUT(float gamma) {

	// Step in uint8_t range for an increase of one "intensity"
	static constexpr float intensity_step = 255.0f / SUNLIGHT_INTENSITY;

	// These are mostly arbitrary values used to curve the light intensity, set by testing done with MineTest 
	// so that things look (objectively) decent and not too dark even at lower gamma.
	static constexpr int adjustments[SUNLIGHT_INTENSITY] = {
		-67, -91, -125, -115, -104, -85, -70, -63, -56, -49, -42, -35, -28, -22, 0,
	};

	// Limit gamma to range.
	if (gamma < 1.0f) {
		gamma = 1.0f;
	}
	if (gamma > 3.0f) {
		gamma = 3.0f;
	}

	// Setup the light LUT
	float intensity = intensity_step;
	for (size_t i = 0; i < MAX_LIGHT_INTENSITY; ++i) {

		Light_LUT[i] = static_cast<uint8_t>(255.0f * powf(intensity / 255.0f, 1.0f / gamma));

		// Add adjustments, making sure we stay in our range.
		if (Light_LUT[i] + adjustments[i] > 255) {
			Light_LUT[i] = 255;
		}
		else if (Light_LUT[i] + adjustments[i] < 0) {
			Light_LUT[i] = 0;
		}
		else {
			Light_LUT[i] += adjustments[i];
		}

		// Make sure values increase along the LUT
		if (i > 1 && Light_LUT[i] < Light_LUT[i - 1]) {
			intensity += intensity_step;
		}
	}

	// Final entry is set to max - specifying the intensity of our sunlight
	Light_LUT[SUNLIGHT_INTENSITY] = 255;
}

// Get uint8_t intensity level for given light
inline uint8_t DecodeLightUI8(uint8_t light) {
	if (light > MAX_LIGHT_INTENSITY) {
		light = MAX_LIGHT_INTENSITY;
	}
	return Light_LUT[light];
}

// Get uint8_t intensity level from a float light value: should be in range of 0.0f - 1.0f
inline float DecodeLightFloat(float light){
	int32_t i = static_cast<int32_t>(light * static_cast<float>(MAX_LIGHT_INTENSITY) + 0.50f);

	if (i <= 0) {
		return static_cast<float>(Light_LUT[0] / 255.0f);
	}
	else if (i >= MAX_LIGHT_INTENSITY) {
		return static_cast<float>(Light_LUT[MAX_LIGHT_INTENSITY] / 255.0f);
	}

	// Get final light value by lerping between two other values (I think...)
	float v1, v2;
	v1 = static_cast<float>(Light_LUT[i - 1] / 255.0f);
	v2 = static_cast<float>(Light_LUT[i] / 255.0f);
	float f0 = static_cast<float>(i) - 0.50f;
	float f = light * static_cast<float>(MAX_LIGHT_INTENSITY) - f0;
	f *= (v2 + (1.0f - f) * v1);
	return f;
}

// Get light intensity given the current time of day as a uint32_t, and the day+night light intensities as u8s
// (Blend Time-of-Day light)
inline uint8_t BlendToDLight(uint32_t time_factor, uint8_t day_light, uint8_t night_light) {
	uint32_t c = 1000;
	uint32_t l = (time_factor * day_light + (c - time_factor) * night_light) / c;
	if (l > SUNLIGHT_INTENSITY) {
		l = SUNLIGHT_INTENSITY;
	}
	return l;
}


#endif // !LIGHT_H
