// HephaestusEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <FastNoiseSIMD.h>
#include "Terrain_Gen.h"
#include <time.h>

typedef float(*arr)[16][16][32];

clock_t t;
int main(){
	t = clock();
	t = clock() - t;
	std::cerr << "Time elapsed to generate terrain was " << static_cast<float>(t) / CLOCKS_PER_SEC << " seconds." << std::endl;
    return 0;
}

