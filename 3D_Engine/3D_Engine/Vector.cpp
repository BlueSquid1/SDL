#include <iostream>
#include "Vector.h"
#include <math.h> 

Vector::Vector()
{
	start.x = 0;
	start.y = 0;
	start.z = 0;

	end.x = 0;
	end.y = 0;
	end.z = 0;
}

float Vector::GetLength()
{
	return sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2) + pow(end.z - start.z, 2));
}


void Vector::GetSlope(float * mReturn)
{
	mReturn[0] = end.x - start.x;
	mReturn[1] = end.y - start.y;
	mReturn[2] = end.z - start.z;
}

void Vector::PrintVec()
{
	float Vec[3];
	this->GetSlope(Vec);

	for (int i = 0; i < 3; i++)
	{
		std::cout << Vec[i] << " ";
	}
	std::cout << std::endl;
}

void Vector::GetUnitVec(float * mReturn)
{
	float len = this->GetLength();
	mReturn[0] = (end.x - start.x) / len;
	mReturn[1] = (end.y - start.y) / len;
	mReturn[2] = (end.z - start.z) / len;
}