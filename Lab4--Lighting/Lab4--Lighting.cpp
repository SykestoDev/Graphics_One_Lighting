// Lab4--Lighting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "RasterFunctions.h"
#include <iostream>
#include "Windows.h"
#include <time.h>

unsigned int redngreen = 0xffffff00;
unsigned int greennblue = 0xff00ffff;

int main()
{
    RS_Initialize("John Hernandez", raster_Width, raster_Height);
	// Camera's(View) Matrix / camera info
	Matrix ViewTranslate = {
		1, 0,  0, 0,
		0, 1,  0, 0,
		0, 0,  1, 0,
		0, 0, -6, 1
	};

	float yScale = 1.0 / (tan(-90) / 2);
	float xScale = yScale / (float(raster_Width) / float(raster_Height));
	float zFar = 10.0;
	float zNear = 0.1;

	// Projection Matrix
	Matrix PerpectiveMatrix = {
		xScale, 0, 0, 0,
		0, yScale, 0, 0,
		0, 0, zFar / (zFar - zNear), 1,
		0, 0, -(zFar * zNear) / (zFar - zNear), 0
	};

	// Set up random points for star field
	Vertex *starFieldArray = new Vertex[3000];
	for (size_t i = 0; i < 3000; i++)
	{
		// Get a random number between -1 & 1 for the x,y,z coordinates + scale by 50 -- keep w = 1
		starFieldArray[i].posArray[0] = (RAND_Float(-1, 1) * 50);
		starFieldArray[i].posArray[1] = (RAND_Float(-1, 1) * 50);
		starFieldArray[i].posArray[2] = (RAND_Float(-1, 1) * 50);
		starFieldArray[i].posArray[3] = 1;
	}

	// Copy over the info from the stoneHeadge.h in order to draw the the mesh
	Vertex *myStoneHedgeArray = new Vertex[1457];
	for (size_t i = 0; i < 1457; i++)
	{
		// copy over the pos coordinates + scale by 0.1 to shrink
		myStoneHedgeArray[i].posArray[0] = StoneHenge_data[i].pos[0] * 0.1f;
		myStoneHedgeArray[i].posArray[1] = StoneHenge_data[i].pos[1] * 0.1f;
		myStoneHedgeArray[i].posArray[2] = StoneHenge_data[i].pos[2] * 0.1f;
		myStoneHedgeArray[i].posArray[3] = 1;
		// copy over the uv coordinates
		myStoneHedgeArray[i].UVarray[0] = StoneHenge_data[i].uvw[0];
		myStoneHedgeArray[i].UVarray[1] = StoneHenge_data[i].uvw[1];
		// copy over the normal coordinates
		myStoneHedgeArray[i].NMarray[0] = StoneHenge_data[i].nrm[0];
		myStoneHedgeArray[i].NMarray[1] = StoneHenge_data[i].nrm[1];
		myStoneHedgeArray[i].NMarray[2] = StoneHenge_data[i].nrm[2];
	}

	// Movement Matrix for Camera
	Matrix TranslateForward = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 2, 1,
	};
	Matrix TranslateBackward = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0,-2, 1,
	};

	// Setup colors for lights
	directionalLight.color = 0xFFC0C0F0;
	pointLight.color = 0xFFFFFF00;

	// Set up the Camera Matrix
	Matrix ViewMatrix = Identity4x4;
	Matrix Camera = Identity4x4;
	Camera = MatrixTimesMatrix(Camera, ViewTranslate);
	Camera = MatrixTimesMatrix(Camera, MatrixRotationX(-0.314));

	do
	{
		unsigned int green = Modulate_Colors(redngreen, greennblue);
		// Clear the screen
		ClearBuffer(0x042d45);
		depthBufffer(0);

		// Set vertex Shader
		VertexShader = VS_MatrixChange;
		SV_WorldMatrix = Identity4x4;

		// Camera Movement
		if (GetAsyncKeyState('A') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, MatrixRotationY(-0.2));
		}
		if (GetAsyncKeyState('D') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, MatrixRotationY(0.2));
		}
		if (GetAsyncKeyState('W') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, MatrixRotationX(0.2));
		}
		if (GetAsyncKeyState('S') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, MatrixRotationX(-0.2));
		}
		if (GetAsyncKeyState('R') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, TranslateForward);
		}
		if (GetAsyncKeyState('F') & 0x01)
		{
			Camera = MatrixTimesMatrix(Camera, TranslateBackward);
		}
		ViewMatrix = InverseMatrix(Camera);
		SV_ViewMatrix = ViewMatrix;

		// Set up projection
		SV_ProjetionMatrix = PerpectiveMatrix;


		PixelShader = PS_White;
		// Set up matrix for Starfield
		Matrix StarFieldMatrix = Identity4x4;
		SV_WorldMatrix = StarFieldMatrix;

		// loop through # of stars
		for (size_t i = 0; i < 3000; i++)
		{
			// Draw the star
			DrawVertex(starFieldArray[i]);
		}

		PixelShader = nullptr;
		// Setup a matrix for the StoneHadge matrix
		Matrix StoneMatrix = Identity4x4;
		SV_WorldMatrix = StoneMatrix;

		// Loop through the # of indices needed to draw the mesh
		for (size_t i = 0; i < 2532;)
		{
			// Draw each triangle using every trio from the indicies array count up by 3
			DrawTriangle(myStoneHedgeArray[StoneHenge_indicies[i++]], myStoneHedgeArray[StoneHenge_indicies[i++]], myStoneHedgeArray[StoneHenge_indicies[i++]]);
		}

	} while (RS_Update(rasterArray, sizeof(rasterArray) >> 2));

	delete[] myStoneHedgeArray;
	delete[] starFieldArray;

	RS_Shutdown();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
