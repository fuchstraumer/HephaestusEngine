// HephaestusEngine.cpp : Defines the entry point for the console application.
//
// Precompiled headers
#include "stdafx.h"
#include "engine\MeshingTest.h"

INITIALIZE_EASYLOGGINGPP
// Other includes.

int main() {
	meshing_test::MeshingScene test_scene;
	test_scene.RenderLoop();
	return 0;
}