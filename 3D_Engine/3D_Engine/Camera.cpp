#include "Camera.h"
#include <math.h>
Camera::Camera()
{
	up.start = this->dirrection.start;
	up.end.x = this->dirrection.start.x;
	up.end.y = this->dirrection.start.x;
	up.end.z = this->dirrection.start.z + 1;
}

void Camera::Pitch(float angle)
{
	/*
	float length = this->dirrection.GetLength();

	dirrection.start.x = length * cos(angle);
	dirrection.start.z = length * sin(angle);
	*/

}

void Camera::Yaw(float angle)
{
	float length = this->dirrection.GetLength();

	dirrection.start.x = length * sin(angle);
	dirrection.start.y = length * cos(angle);

	up.start = dirrection.start;
	up.end = up.start;
	up.end.z = up.end.z + 1;
}