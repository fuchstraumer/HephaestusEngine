#pragma once
#ifndef HEPHAESTUS_ENGINE_GUI_ELEMENTS_H
#define HEPHAESTUS_ENGINE_GUI_ELEMENTS_H

#include "stdafx.h"
#include "util/imguiTabs.hpp"
#include "core/Instance.hpp"

static const char* tab_names[] = { "Lighting", "Renderer Settings", "Chunk Settings", "Debug Utilities" };
static int tab_order[] = { 0, 1, 2, 3 };
static int tab_active = 0;


enum class possibleTabs : int {
	LIGHTING,
	RENDERER_SETTINGS,
	CHUNK_SETTINGS,
	DEBUG_UTILITIES
};

static glm::vec3 lightPos = glm::vec3(0.0f, 500.0f, 0.0f);
static glm::vec3 lightColor = glm::vec3(0.95f, 0.98f, 0.95f);

static void DrawLightingSettings() {
	ImGui::PushID("Lighting");
	ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 1.0f, -500.0f, 500.0f);
	ImGui::DragFloat3("Light Color", glm::value_ptr(lightColor), 0.01f, 0.0f, 1.0f);
	ImGui::PopID();
}

static const char* possible_cameras[] = { "FPS", "Arcball" };
static int msaa_value = vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount;
static const char* possible_msaa_values[] = { "1", "2", "4", "8", "16" };

static void DrawRendererSettings() {
	ImGui::PushID("Renderer Settings");
	int* current_camera = reinterpret_cast<int*>(&vulpes::Instance::VulpesInstanceConfig.CameraType);
	ImGui::Combo("Camera type", current_camera, possible_cameras, 2);

	ImGui::Checkbox("Enable MSAA", &vulpes::Instance::VulpesInstanceConfig.EnableMSAA);

	switch (vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount) {
	case VK_SAMPLE_COUNT_1_BIT:
		msaa_value = 0;
		break;
	case VK_SAMPLE_COUNT_2_BIT:
		msaa_value = 1;
		break;
	case VK_SAMPLE_COUNT_4_BIT:
		msaa_value = 2;
		break;
	case VK_SAMPLE_COUNT_8_BIT:
		msaa_value = 3;
		break;
	case VK_SAMPLE_COUNT_16_BIT:
		msaa_value = 4;
		break;
	}

	ImGui::SameLine();
	ImGui::Combo("MSAA Samples", &msaa_value, possible_msaa_values, 5);

	switch (msaa_value) {
	case 0:
		vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount = VK_SAMPLE_COUNT_1_BIT;
		break;
	case 1:
		vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount = VK_SAMPLE_COUNT_2_BIT;
		break;
	case 2:
		vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount = VK_SAMPLE_COUNT_4_BIT;
		break;
	case 3:
		vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount = VK_SAMPLE_COUNT_8_BIT;
		break;
	case 4:
		vulpes::Instance::VulpesInstanceConfig.MSAA_SampleCount = VK_SAMPLE_COUNT_16_BIT;
		break;
	}

	ImGui::Separator();
	ImGui::Checkbox("Enable Mouse Locking", &vulpes::Instance::VulpesInstanceConfig.EnableMouseLocking);
	ImGui::Checkbox("Limit Framerate", &vulpes::Instance::VulpesInstanceConfig.LimitFramerate);
	ImGui::DragFloat("Framerate limit in Ms", &vulpes::Instance::VulpesInstanceConfig.FrameTimeMs, 0.01f, 4.0f, 32.0f);
	ImGui::Text("Use this button to update graphics settings."); ImGui::SameLine(); 
	if (ImGui::Button("Refresh")) {
		vulpes::Instance::VulpesInstanceConfig.RequestRefresh = true;
	}
	ImGui::DragFloat("Movement Speed", &vulpes::Instance::VulpesInstanceConfig.MovementSpeed, 0.01f, 0.0001f, 10.0f, "%.4f");
	ImGui::PopID();
}

static int render_distance = 8;

static void DrawChunkSettings() {
	ImGui::PushID("Chunk Settings");
	ImGui::DragInt("Render Distance", &render_distance, 0.2f, 2, 16);
	ImGui::PopID();
}

static void DrawDebugUtils() {

}

static void DrawGUI() {

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("GUI Root");

	const bool tab_changed = ImGui::TabLabels(tab_names, sizeof(tab_names) / sizeof(tab_names[0]), tab_active, tab_order);

	switch (static_cast<possibleTabs>(tab_active)) {
	case possibleTabs::LIGHTING:
		DrawLightingSettings();
		break;
	case possibleTabs::RENDERER_SETTINGS:
		DrawRendererSettings();
		break;
	case possibleTabs::CHUNK_SETTINGS:
		DrawChunkSettings();
		break;
	case possibleTabs::DEBUG_UTILITIES:
		DrawDebugUtils();
		break;
	default:
		DrawRendererSettings();
		break;
	}

	ImGui::End();
}

#endif // !HEPHAESTUS_ENGINE_GUI_ELEMENTS_H
