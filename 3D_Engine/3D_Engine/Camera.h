#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"

class Camera
{
public:
	Camera();
	Vector dirrection;
	Vector up;
	void Pitch(float angle);
	void Yaw(float angle);
};

#endif