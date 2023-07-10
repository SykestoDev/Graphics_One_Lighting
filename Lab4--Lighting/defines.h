#pragma once
#include "RasterSurface.h"
#include <vector>
#include "StoneHenge.h"
#include "StoneHenge_Texture.h"

// Screen Info
const unsigned int raster_Width = 1000;
const unsigned int raster_Height = 600;
const unsigned int numpixels = raster_Width * raster_Height;
unsigned int rasterArray[numpixels];

float zArray[numpixels];


struct Vertex
{
	float posArray[4] = { 0, 0, 0, 0 };
	float UVarray[2]  = { 0, 0 };
	float NMarray[3]  = { 0, 0, 0 };
	unsigned int color;
};

struct vec3
{
	float xyz[3] = { 0, 0, 0 };
};

// Matrix
struct Matrix
{
	float matrixArray[4][4] = { 1, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1 
	};

};




