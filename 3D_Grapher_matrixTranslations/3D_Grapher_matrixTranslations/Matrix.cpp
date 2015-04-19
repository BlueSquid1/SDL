#include "Matrix.h"
#include <iostream>

Matrix::Matrix(int y, int x)
{
	SetGridDim(y, x);
}

bool Matrix::SetGridDim(int y, int x)
{
	grid = new float*[y];
	for (int i = 0; i < y; ++i)
	{
		grid[i] = new float[x];
	}
	sizeY = y;
	sizeX = x;
	return true;
}


Matrix operator*(Matrix &A, Matrix &B)
{
	Matrix temp(A.sizeY, B.sizeX);

	//check the dimensions first
	if (A.sizeX != B.sizeY)
	{
		std::cout << "dimensions are wrong";
		return temp;
	}

	for (int c = 0; c < A.sizeY; c++)
	{
		for (int b = 0; b < B.sizeX; b++)
		{
			float total = 0;
			for (int a = 0; a < A.sizeX; a++)
			{
				total += A.grid[c][a] * B.grid[a][b];
			}
			temp.grid[c][b] = total;
		}
	}

	return temp;
}

void Matrix::Print()
{
	for (int y = 0; y < sizeY; y++)
	{
		std::cout << "[ ";
		for (int x = 0; x < sizeX; x++)
		{
			std::cout << grid[y][x] << " ";
		}
		std::cout << "]\n";
	}
}