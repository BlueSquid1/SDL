#ifndef VECTOR_H
#define VECTOR_H

#include "Point.h"

class Vector
{
public:
	Vector();

	Point start;
	Point end;

	void Pitch(float angle);
	void Yaw(float angle);

	float GetLength();
	void GetSlope(float * mReturn);
	void GetUnitVec(float * mReturn);

	void PrintVec();
};
#endif