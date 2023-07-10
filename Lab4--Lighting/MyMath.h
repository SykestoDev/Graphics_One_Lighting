#pragma once
#include "defines.h"
#include <cmath>
#include <iostream>
#include <math.h>


// Identity 4x4 matrix
Matrix Identity4x4 = { 1, 0, 0, 0,
					   0, 1, 0, 0,
					   0, 0, 1, 0,
					   0, 0, 0, 1

};

// Matrix Rotation on the X axis
Matrix MatrixRotationX(float angle)
{
	Matrix RotateX = { 1,	 0,	          0,       0,
					   0, cos(angle), -sin(angle), 0,
					   0, sin(angle),  cos(angle), 0,
					   0,    0,           0,       1
	};

	return RotateX;
}
// Matrix Rotation on the Y axis
Matrix MatrixRotationY(float angle)
{
	Matrix RotateY = { cos(angle), 0, sin(angle), 0,
						   0,      1,      0,     0,
					  -sin(angle), 0, cos(angle), 0,
						   0,      0,      0,     1
	};

	return RotateY;
}
// Matrix Rotation on the Z axis
Matrix RoatateMatrirxZ(float angle)
{
	Matrix RotateZ = { cos(angle),-sin(angle), 0, 0,
					   sin(angle), cos(angle), 0, 0,
					      0,          0,       1, 0,
					      0,          0,       0, 1
	};

	return RotateZ;
}


// vec4 { X, Y, Z, W } x Vs_Matrix { A, B, C, D }
//                                 { E, F, G, H }
//								   { I, J, K, L }
//								   { M, N, O, P }
//
//
Vertex VectorTimesMatrix(Vertex vect, Matrix matrix)
{
	Vertex VectorProduct = vect;

	for (size_t matrixRow = 0; matrixRow < 4; matrixRow++)
	{
		float sumElements = 0;
		for (size_t vecRows = 0; vecRows < 4; vecRows++)
		{
			sumElements += vect.posArray[vecRows] * matrix.matrixArray[vecRows][matrixRow];
		}
		VectorProduct.posArray[matrixRow] = sumElements;
	}
	return VectorProduct;
}


// VS_Matrix1 { A, B, C, D } x Vs_Matrix2 { Q, R, S, T } //
//			  { E, F, G, H }			  { U, V, W, X } //
//			  { I, J, K, L } 		      { Y, Z, 0, 1 } //
//		      { M, N, O, P }			  { 2, 3, 4, 5 } //
//													     //
//													     //
Matrix MatrixTimesMatrix(Matrix _1matrix, Matrix _2matrix)
{
	Matrix MatrixProduct;

	for (size_t matrix1row = 0; matrix1row < 4; matrix1row++)
	{
		for (size_t matrix2row = 0; matrix2row < 4; matrix2row++)
		{
			float sumElements = 0;

			for (size_t col = 0; col < 4; col++)
			{
				sumElements += _1matrix.matrixArray[matrix1row][col] * _2matrix.matrixArray[col][matrix2row];
			}

			MatrixProduct.matrixArray[matrix1row][matrix2row] = sumElements;
		}
	}

	return  MatrixProduct;
}

Vertex Vec3xMatrix3(Vertex vector3, Matrix matrix3x3)
{
	Vertex VectorProduct = vector3;
	//				x					   x                         a                           y                         d                           z                         g
	VectorProduct.posArray[0] = (vector3.posArray[0] * matrix3x3.matrixArray[0][0]) + (vector3.posArray[1] * matrix3x3.matrixArray[1][0]) + (vector3.posArray[2] * matrix3x3.matrixArray[2][0]);
	//				y					                             b                                                     e                                                     h
	VectorProduct.posArray[1] = (vector3.posArray[0] * matrix3x3.matrixArray[0][1]) + (vector3.posArray[1] * matrix3x3.matrixArray[1][1]) + (vector3.posArray[2] * matrix3x3.matrixArray[2][1]);
	//				z					                             c                                                     f                                                     i
	VectorProduct.posArray[2] = (vector3.posArray[0] * matrix3x3.matrixArray[0][2]) + (vector3.posArray[1] * matrix3x3.matrixArray[1][2]) + (vector3.posArray[2] * matrix3x3.matrixArray[2][2]);

	for (size_t i = 0; i < 3; i++)
	{
		VectorProduct.posArray[i] = VectorProduct.posArray[i] * -1;
	}
	return VectorProduct;
}

// return the Inverse of a matrix
Matrix InverseMatrix(Matrix inverseMe)
{
	// Section off 3x3 from our 4x4 
	Matrix matrixInversed;
	matrixInversed.matrixArray[0][0] = inverseMe.matrixArray[0][0];
	matrixInversed.matrixArray[0][1] = inverseMe.matrixArray[1][0];
	matrixInversed.matrixArray[0][2] = inverseMe.matrixArray[2][0];

	matrixInversed.matrixArray[1][0] = inverseMe.matrixArray[0][1];
	matrixInversed.matrixArray[1][1] = inverseMe.matrixArray[1][1];
	matrixInversed.matrixArray[1][2] = inverseMe.matrixArray[2][1];

	matrixInversed.matrixArray[2][0] = inverseMe.matrixArray[0][2];
	matrixInversed.matrixArray[2][1] = inverseMe.matrixArray[1][2];
	matrixInversed.matrixArray[2][2] = inverseMe.matrixArray[2][2];
	
	// Section off last row from our 4x4 
	Vertex Vector3;
	Vector3.posArray[0] = inverseMe.matrixArray[3][0];
	Vector3.posArray[1] = inverseMe.matrixArray[3][1];
	Vector3.posArray[2] = inverseMe.matrixArray[3][2];

	// Multiply the two splited sections
	 Vector3 = Vec3xMatrix3(Vector3, matrixInversed);

	// Join the splitted row into the original 4x4
	matrixInversed.matrixArray[3][0] = Vector3.posArray[0];
	matrixInversed.matrixArray[3][1] = Vector3.posArray[1];
	matrixInversed.matrixArray[3][2] = Vector3.posArray[2];
	
	return matrixInversed;
}

// Dot product
float DotProduct(Vertex V1, Vertex V2)
{
	float dot = 0;
	
	for (size_t i = 0; i < 3; i++)
	{
		dot += (V1.posArray[i] * V2.NMarray[i]);
	}

	return dot;
}

// Cross product
Vertex CrossProduct(Vertex V1, Vertex V2)
{
	Vertex tempV{ 0 , 0, 0 };
	// x              =        v1.y     *       v2.z      -        v1.z     *       v2.y
	tempV.posArray[0] = (V1.posArray[1] * V2.posArray[2]) - (V1.posArray[2] * V2.posArray[1]);
	// y              =        v1.z     *       v2.x      -        v1.x     *       v2.z
	tempV.posArray[1] = (V1.posArray[2] * V2.posArray[0]) - (V1.posArray[0] * V2.posArray[2]);
	// z              =        v1.x     *       v2.y      -        v1.y     *       v2.x
	tempV.posArray[2] = (V1.posArray[0] * V2.posArray[1]) - (V1.posArray[1] * V2.posArray[0]);
	return tempV;
}

// vec3 normalize - NMarray
Vertex Vec3_Normalize(Vertex V)
{
	// find magnitude of vector
	float length = sqrt((V.NMarray[0] * V.NMarray[0]) + (V.NMarray[1] * V.NMarray[1]));

	if (length != 0)
	{
		// Divide each coordinate by the magniude 
		V.NMarray[0] = V.NMarray[0] / length;
		V.NMarray[1] = V.NMarray[1] / length;
		//V.posArray[2] = V.posArray[2] / length;
	}

	return V;
}
// vec3 normalize -- posArray
Vertex Vec3_PNormalize(Vertex V)
{
	// find magnitude of vector
	float length = sqrt((V.posArray[0] * V.posArray[0]) + (V.posArray[1] * V.posArray[1]) + (V.posArray[2] * V.posArray[2]));

	if (length != 0)
	{
		// Divide each coordinate by the magniude 
		V.posArray[0] = V.posArray[0] / length;
		V.posArray[1] = V.posArray[1] / length;
		V.posArray[2] = V.posArray[2] / length;
	}

	return V;
}

// Saturate Colors
float Saturate(float num)
{
	if (num < 0)
	{
		return num = 0;
	}
	else if (num > 1)
	{
		return num = 1;
	}
	else
	{
		return num;
	}
}

// Combine Colors
unsigned int CombineColor(unsigned int color1, unsigned int color2)
{
	// Format BGRA - Break both colors into color channels + bit shift
	int alpha_channel = (color1 & 0xff000000) >> 24;
	int red_channel   = (color1 & 0x00ff0000) >> 16;
	int green_channel = (color1 & 0x0000ff00) >> 8;
	int blue_channel  = (color1 & 0x000000ff) >> 0;

	int Alpha_channel = (color2 & 0xff000000) >> 24;
	int Red_channel   = (color2 & 0x00ff0000) >> 16;
	int Green_channel = (color2 & 0x0000ff00) >> 8;
	int Blue_channel  = (color2 & 0x000000ff) >> 0;

	// ratio should be divided by the max to get a ratio of 0 - 1 for color1
	float ar = alpha_channel / 255.0f;
	float rr = red_channel   / 255.0f;
	float gr = green_channel / 255.0f;
	float br = blue_channel  / 255.0f;

	// ratio should be divided by the max to get a ratio of 0 - 1 for color2
	float Ar = Alpha_channel / 255.0f;
	float Rr = Red_channel / 255.0f;
	float Gr = Green_channel / 255.0f;
	float Br = Blue_channel / 255.0f;

	// Combine the colors of each color channel
	float finalAlpha = ar + Ar;
	float finalRed   = rr + Rr;
	float finalGreen = gr + Gr;
	float finalBlue  = br + Br;

	finalAlpha = 255 * (Saturate(finalAlpha));
	finalRed   = 255 * (Saturate(finalRed));
	finalGreen = 255 * (Saturate(finalGreen));
	finalBlue  = 255 * (Saturate(finalBlue));

	

	// Combine the channels into 1 color
	return ((int)finalAlpha << 24) | ((int)finalRed << 16) | ((int)finalGreen << 8) | (int)finalBlue;
}

// Modulate colors
unsigned int Modulate_Colors(unsigned int color1, unsigned int color2)
{
	// Format XRGB - Break both colors into color channels + bit shift
	int alpha_channel = (color1 & 0xff000000) >> 24;
	int red_channel   = (color1 & 0x00ff0000) >> 16;
	int green_channel = (color1 & 0x0000ff00) >> 8;
	int blue_channel  = (color1 & 0x000000ff) >> 0;

	int Alpha_channel = (color2 & 0xff000000) >> 24;
	int Red_channel   = (color2 & 0x00ff0000) >> 16;
	int Green_channel = (color2 & 0x0000ff00) >> 8;
	int Blue_channel  = (color2 & 0x000000ff) >> 0;

	// ratio should be divided by the max to get a ratio of 0 - 1 for color1
	float ar = alpha_channel / 255.0f;
	float rr = red_channel   / 255.0f;
	float gr = green_channel / 255.0f;
	float br = blue_channel  / 255.0f;

	// ratio should be divided by the max to get a ratio of 0 - 1 for color2
	float Ar = Alpha_channel / 255.0f;
	float Rr = Red_channel   / 255.0f;
	float Gr = Green_channel / 255.0f;
	float Br = Blue_channel  / 255.0f;

	// Combine the colors of each color channel
	float finalAlpha = ar * Ar;
	float finalRed   = rr * Rr;
	float finalGreen = gr * Gr;
	float finalBlue  = br * Br;

	finalAlpha = 255 * (Saturate(finalAlpha));
	finalRed   = 255 * (Saturate(finalRed));
	finalGreen = 255 * (Saturate(finalGreen));
	finalBlue  = 255 * (Saturate(finalBlue));

	// Combine the channels into 1 color
	return ((int)finalAlpha << 24) | ((int)finalRed << 16) | ((int)finalGreen << 8) | (int)finalBlue;
}