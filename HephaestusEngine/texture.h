#ifndef TEXTURE_H

#define TEXTURE_H

#include "stdafx.h"
#include "lodepng.h"
typedef unsigned int uint;
typedef unsigned char uchar;

void flip_img_vertical(uchar *data, uint width, uint height) {
	uint size = width*height * 4; uint stride = sizeof(char) * width * 4;
	unsigned char *new_data = malloc(sizeof(uchar) * size);
}

void load_PNG(const char *file) {
	uint error; uchar *data; uint width, height;
	error = lodepng_decode32_file(&data,&width,&height,file);


}

#endif