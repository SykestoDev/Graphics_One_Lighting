#pragma once
//#include "defines.h"
#include "MyMath.h"
#include <algorithm>

// Light source 
Vertex directionalLight = { -0.577, -0.577, 0.577 };
Vertex pointLight = { -1, 0.5, 1 };

// Function pointers
void (*VertexShader)(Vertex&) = nullptr;
void (*PixelShader)(unsigned int&) = nullptr;

Matrix SV_WorldMatrix;
Matrix SV_ViewMatrix;
Matrix SV_ProjetionMatrix;

// Linear interplolation between p1 and p2
float Lerp(float start, float end, float t)
{
	// P1 + R(P2 - P1)
	return start + (end - start) * t;
}

float Color_Lerp(unsigned int color1, unsigned int color2, float r)
{
	int alpha_channel = (color1 & 0x000000ff) >> 0;
	int red_channel   = (color1 & 0x0000ff00) >> 8;
	int green_channel = (color1 & 0x00ff0000) >> 16;
	int blue_channel  = (color1 & 0xff000000) >> 24;

	int Alpha_channel = (color2 & 0x000000ff) >> 0;
	int Red_channel   = (color2 & 0x0000ff00) >> 8;
	int Green_channel = (color2 & 0x00ff0000) >> 16;
	int Blue_channel  = (color2 & 0xff000000) >> 24;

	// Linear interpolation of each color channel
	float finalAlpha = alpha_channel + (Alpha_channel - alpha_channel) * r;
	float finalRed   = red_channel + (Red_channel - red_channel) * r;
	float finalGreen = green_channel + (Green_channel - green_channel) * r;
	float finalBlue  = blue_channel + (Blue_channel - blue_channel) * r;

	// Combine the channels into 1 color
	return ((int)finalAlpha << 0) | ((int)finalRed << 8) | ((int)finalGreen << 16) | ((int)finalBlue << 24);
}
// Vertex Shaders
void VS_MatrixChange(Vertex& changeMe)
{
	changeMe = VectorTimesMatrix(changeMe, SV_WorldMatrix);

	Vertex Normal = Vec3_Normalize(changeMe);
	Normal = VectorTimesMatrix(Normal, SV_WorldMatrix);
	// Negate XYZ nm
	Normal.NMarray[0] = -Normal.NMarray[0];
	Normal.NMarray[1] = -Normal.NMarray[1];
	Normal.NMarray[2] = -Normal.NMarray[2];
	float lightRatio = Saturate(DotProduct(directionalLight, Normal));
	unsigned int dColor = Color_Lerp(0x00000000, directionalLight.color, lightRatio); // colorlerp

	Vertex lightDir;
	lightDir.posArray[0] = -(pointLight.posArray[0] - changeMe.posArray[0]);
	lightDir.posArray[1] = -(pointLight.posArray[1] - changeMe.posArray[1]);
	lightDir.posArray[2] = -(pointLight.posArray[2] - changeMe.posArray[2]);
	lightDir = Vec3_PNormalize(lightDir);
	lightDir = VectorTimesMatrix(lightDir, SV_WorldMatrix);
	float plightRatio = Saturate(DotProduct(lightDir, Normal));
	unsigned int pColor = Color_Lerp(0x00000000, pointLight.color, plightRatio);
	changeMe.color = CombineColor(dColor, pColor);

	changeMe = VectorTimesMatrix(changeMe, SV_ViewMatrix);
	changeMe = VectorTimesMatrix(changeMe, SV_ProjetionMatrix);
	// Perpective divide
	changeMe.posArray[0] = changeMe.posArray[0] / changeMe.posArray[3];
	changeMe.posArray[1] = changeMe.posArray[1] / changeMe.posArray[3];
	changeMe.posArray[2] = changeMe.posArray[2] / changeMe.posArray[3];
}

// Pixel Shaders
void PS_Blue(unsigned int& changeMe)
{
	changeMe = 0xff0000ff;
}

void PS_Green(unsigned int& changeMe)
{
	changeMe = 0xff00ff00;
}

void PS_Red(unsigned int& changeMe)
{
	changeMe = 0xffff0000;
}

void PS_White(unsigned int& changeMe)
{
	changeMe = 0x00ffffff;
}

void PS_Yellow(unsigned int& changeMe)
{
	changeMe = 0xffffff00;
}

void PS_Cyan(unsigned int& changeMe)
{
	changeMe = 0xff00ffff;
}

void PS_Purple(unsigned int& changeMe)
{
	changeMe = 0x00ff00ff;
}