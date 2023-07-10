#pragma once

#include <iostream>
#include "Shaders.h"


// Texture points
Vertex P1 = { 0  , 0   };
Vertex P2 = { 512, 512 };
Vertex P3 = { 0  , 512 };
// Clear the screen setting all points in the array to a color
void ClearBuffer(unsigned int color)
{
	for (size_t i = 0; i < raster_Width*raster_Height; i++)
	{
		rasterArray[i] = color;
	}
}

void depthBufffer(unsigned int color)
{

	for (size_t i = 0; i < raster_Width * raster_Height; i++)
	{
		zArray[i] = 100;
	}
}

int twoDtoOneD(int texture_widtth, int x, int y)
{
	// Formula(Y * width + X)
	int oneDresult = (y * texture_widtth + x);
	return oneDresult;

}

// Plot a pixel to the screen on an(x,y) postion
void PlotPixel(int destination_x, int destination_y, float destination_z, unsigned int color)
{

	// Have something check for the boundary of the window
	if (destination_x >= 0 && destination_x <= raster_Width && destination_y >= 0 && destination_y <= raster_Height)
	{
		int index = twoDtoOneD(raster_Width,destination_x, destination_y);

		if (destination_z <= zArray[index])
		{
			// Y * Width + X
			zArray[index] = destination_z;
			rasterArray[index] = color;
		}
	}
}

// Convert NDC coordinates to screen coordinates
Vertex NDCtoScreenCoordinates(Vertex screen_vector)
{
	Vertex ScreenXY = screen_vector;
	// point + 1 * (length / 2)
	ScreenXY.posArray[0] = floor((screen_vector.posArray[0] + 1) * (raster_Width / 2));
	ScreenXY.posArray[1] = floor((1 - screen_vector.posArray[1]) * (raster_Height / 2));
	return ScreenXY;
}

// Use parametric equation to draw line
void ParametricLine(Vertex startPoint, Vertex endPoint)
{
	// Copy the line (don't use original)
	Vertex CopyStartpoint = startPoint;
	Vertex CopyEndpoint = endPoint;

	// Distance between the two points
	float deltaX = abs(CopyEndpoint.posArray[0] - CopyStartpoint.posArray[0]);
	float deltaY = abs(CopyEndpoint.posArray[1] - CopyStartpoint.posArray[1]);

	float lineLength = 0;
	// Check to see which linelength distance is greater between the X and Y 
	if (deltaX > deltaY)
	{
		lineLength = deltaX;
	}
	else
	{
		lineLength = deltaY;
	}

	// loop using whichever line was greater
	for (size_t i = 0; i < lineLength; i++)
	{
		// Current ratio of line 
		float ratio = (float)i / lineLength;

		// Lerp for X and Y
		float Xcurrent = Lerp(startPoint.posArray[0], endPoint.posArray[0], ratio);
		float Ycurrent = Lerp(startPoint.posArray[1], endPoint.posArray[1], ratio);
		float Zcurrent = Lerp(startPoint.posArray[2], endPoint.posArray[2], ratio);

		// Assign color - Call Function pointer
		unsigned int copyColor = startPoint.color;
		if (PixelShader != nullptr)
		{
			PixelShader(copyColor);
		}

		// PLot Pixel based on the current posistion
		PlotPixel(Xcurrent + .5, Ycurrent + .5, Zcurrent + 0.5, copyColor);
	}
}

// Draw line converting NDC to the screen 
void DrawLine(const Vertex &startpoint, const Vertex &endpoint)
{
	// Make copies of points
	Vertex CopyStartpoint = startpoint;
	Vertex CopyEndpoint = endpoint;

	// Check to see if the vertex shader isnt null
	if (VertexShader != nullptr)
	{
		VertexShader(CopyStartpoint);
		VertexShader(CopyEndpoint);
	}
	
	// Convert point from NDC coordinates to screen coordinates
	Vertex ScreenStart =  NDCtoScreenCoordinates(CopyStartpoint);
	Vertex ScreenEnd = NDCtoScreenCoordinates(CopyEndpoint);

	// Draw line
	ParametricLine(ScreenStart, ScreenEnd);
}

// Get the minimum between 2 numbers
float min(float one, float two)
{
	if (one < two)
	{
		return one;
	}
	else
	{
		return two;
	}
}

// get the max between 2 numbers
float max(float one, float two)
{
	if (one > two)
	{
		return one;
	}
	else
	{
		return two;
	}
}

// Implicite Line Equation
float ImplicitLineEquation(Vertex A, Vertex B, Vertex p)
{
	return ((A.posArray[1] - B.posArray[1]) * p.posArray[0] ) + ((B.posArray[0] - A.posArray[0]) * p.posArray[1]) + ((A.posArray[0] * B.posArray[1]) - (A.posArray[1] * B.posArray[0]));
}


Vertex GetBarycentricCoor(Vertex pointA, Vertex pointB, Vertex pointC, Vertex pointP)
{
	float maxA = ImplicitLineEquation(pointA, pointB, pointC);
	float subA = ImplicitLineEquation(pointP, pointB, pointC);

	float maxB = ImplicitLineEquation(pointB, pointC, pointA);
	float subB = ImplicitLineEquation(pointP, pointC, pointA);

	float maxC = ImplicitLineEquation(pointC, pointB, pointA);
	float subC = ImplicitLineEquation(pointP, pointB, pointA);

	return { subA / maxA, subB / maxB, subC / maxC };

}

// Takes in BGRA format and chanes it to XRGB format
unsigned int ChangeColorFormat(unsigned int color)
{
	// Format coming in BGRA + Split into channels
	unsigned int Blue  = (0xff000000 & color) >> 24;
	unsigned int Green = (0x00ff0000 & color) >> 16;
	unsigned int Red   = (0x0000ff00 & color) >> 8;
	unsigned int Alpha = (0x000000ff & color) >> 0;

	// Return the color in the XRGB format
	return (Alpha << 24) | (Red << 16) | (Green << 8) | Blue;
}

// Takes an area from an image_source(array) and displays those pixles to the screen
void Blit(int rasterPos_x, int rasterPos_y, const unsigned int* textureColorArray, unsigned int left , unsigned int top, unsigned int right, unsigned int bottom, unsigned int texture_width)
{
	// Get distance between the width endpoints as well as the height endpoints
	int horizontalDistance = right - left;
	int verticalDistance = bottom - top;

	// Iterate through the difference in both the vertical distance(y2 - y1) and horizontal distance(x2 - x1)
	for (size_t y = 0; y < verticalDistance; y++)
	{
		for (size_t x = 0; x < horizontalDistance; x++)
		{
			// Turn 2d position to 1d position  
			// Save the index 
			int index = twoDtoOneD(texture_width, left + x, top + y);
			int block = textureColorArray[index];

			// Change Color Format
			block = ChangeColorFormat(block);

			// Draw the Pixel
			PlotPixel(rasterPos_x + x, rasterPos_y + y, 0, block);
		}
	}
}

float BaryLerp(float A, float B, float C, Vertex baryPoint)
{
	return (A * baryPoint.posArray[0]) + (B * baryPoint.posArray[1]) + (C * baryPoint.posArray[2]);
}
unsigned int BaryColorLerp(unsigned int color1, unsigned int color2, unsigned int color3, float xRatio, float yRatio, float zRatio)
{
	unsigned int Alpha = (0xff000000 & color1) >> 24;
	unsigned int Red   = (0x00ff0000 & color1) >> 16;
	unsigned int Green = (0x0000ff00 & color1) >> 8;
	unsigned int Blue  = (0x000000ff & color1) >> 0;

	unsigned int alpha = (0xff000000 & color2) >> 24;
	unsigned int red   = (0x00ff0000 & color2) >> 16;
	unsigned int green = (0x0000ff00 & color2) >> 8;
	unsigned int blue  = (0x000000ff & color2) >> 0;

	unsigned int aAlpha = (0xff000000 & color3) >> 24;
	unsigned int rRed   = (0x00ff0000 & color3) >> 16;
	unsigned int gGreen = (0x0000ff00 & color3) >> 8;
	unsigned int bBlue  = (0x000000ff & color3) >> 0;

	int finalAlpha = (Alpha * xRatio) + (alpha * yRatio) + (aAlpha * zRatio);
	int finalRed   = (Red * xRatio) + (red * yRatio) + (rRed * zRatio);
	int finalBlue  = (Blue * xRatio) + (blue * yRatio) + (bBlue * zRatio);
	int finalGreen = (Green * xRatio) + (Green * yRatio) + (gGreen * zRatio);

	return  (finalAlpha << 24) | (finalRed << 16) | (finalGreen << 8) | finalBlue;
}
// Better Brute 
void FillTriangle(Vertex A, Vertex B, Vertex C)
{
	// store the min and max of the 3 sets of coordinates
	float minX = min(A.posArray[0], min(B.posArray[0], C.posArray[0]));
	float maxX = max(A.posArray[0], max(B.posArray[0], C.posArray[0]));
	float minY = min(A.posArray[1], min(B.posArray[1], C.posArray[1]));
	float maxY = max(A.posArray[1], max(B.posArray[1], C.posArray[1]));

	// loop through the min and max for both X and Y
	for (float y = minY; y <= maxY; y++)
	{
		for (float x = minX; x <= maxX; x++)
		{
			// get barycentric coordinates
			Vertex bary = GetBarycentricCoor(A, B, C, { x, y});

			if (bary.posArray[0] >= 0 && bary.posArray [0] <= 1 && bary.posArray[1] >= 0 && bary.posArray[1] <= 1 && bary.posArray[2] >= 0 && bary.posArray[2] <= 1)
			{
				float z = (A.posArray[2] * bary.posArray[0]) + (B.posArray[2] * bary.posArray[1]) + (C.posArray[2] * bary.posArray[2]);

				//Calculate U and V the same as Z as above
				float baryU = BaryLerp(A.UVarray[0], B.UVarray[0], C.UVarray[0], bary);
				float baryV = BaryLerp(A.UVarray[1], B.UVarray[1], C.UVarray[1], bary);

				//Multilpy U times texture_Width and V * texturte_Height to get texture coordinates to get color from texture
				float textureU = baryU * StoneHenge_width;
				float textureV = baryV * StoneHenge_height;

				int index = twoDtoOneD(StoneHenge_width, textureU, textureV);
				unsigned int texture = ChangeColorFormat(StoneHenge_pixels[index]);

				unsigned int barycolor = BaryColorLerp(A.color, B.color, C.color, bary.posArray[0], bary.posArray[1], bary.posArray[2]);

				if (PixelShader != nullptr)
				{
					PixelShader(texture);
				}
				unsigned int finalColor = Modulate_Colors(barycolor, texture);

				PlotPixel(x, y, z, finalColor);
			}
		}
	}
}

// get 3 points and draw a triangle as well as fill the triangle
void DrawTriangle( Vertex A, Vertex B, Vertex C)
{
	// Make copies of points
	Vertex CopypointA = A;
	Vertex CopypointB = B;
	Vertex CopyPointC = C;

	// Check to see if the vertex shader isnt null
	if (VertexShader != nullptr)
	{
		VertexShader(CopypointA);
		VertexShader(CopypointB);
		VertexShader(CopyPointC);
	}
	

	// Convert point from NDC coordinates to screen coordinates
	Vertex ScreenA = NDCtoScreenCoordinates(CopypointA);
	Vertex ScreenB = NDCtoScreenCoordinates(CopypointB);
	Vertex ScreenC = NDCtoScreenCoordinates(CopyPointC);

	FillTriangle(ScreenA, ScreenB, ScreenC);

	//// Draw line
	ParametricLine(ScreenA, ScreenB);
	ParametricLine(ScreenB, ScreenC);
	ParametricLine(ScreenC, ScreenA);

}

// Get random number between a min and a max
float RAND_Float(float min, float max)
{
	float r = rand() / float(RAND_MAX);
	return Lerp(min, max, r);
}
// Plotting random pixels on the screen
void PlotRandom(unsigned int color)
{
	Vertex rP;
	//srand((unsigned)time(0));
	rP.posArray[1] = RAND_Float(-1, 1);
	rP.posArray[0] = RAND_Float(-1, 1);

	rP = NDCtoScreenCoordinates(rP);

	PlotPixel(rP.posArray[0], rP.posArray[1], 1, color);
}

void DrawVertex(Vertex v)
{
	Vertex CopypointV = v;

	if (VertexShader != nullptr)
	{
		VertexShader(CopypointV);
	}

	Vertex ScreenpointV = NDCtoScreenCoordinates(CopypointV);

	// Assign color - Call Function pointer
	unsigned int color = 0;

	if (PixelShader != nullptr)
	{
		PixelShader(color);
	}

	PlotPixel(ScreenpointV.posArray[0], ScreenpointV.posArray[1], ScreenpointV.posArray[2], color);
}
