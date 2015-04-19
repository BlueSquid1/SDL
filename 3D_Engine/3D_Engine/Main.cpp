#include <iostream>
#include <SDL.h>
#include <math.h> 

#include "Point.h"
#include "Vector.h"
#include "camera.h"


//global variables
//----------------
const int SCREEN_HEIGHT = 400;
const int SCREEN_WIDTH = 600;

SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
float M2P = 40;
float P2M = 1 / M2P;
//================


//functions
//---------
bool Init();
float DotProduct(float * vec1, float * vec2);
void CrossProduct(float * mReturn, float * vec1, float * vec2);
void ScaleProduct(float scale, float * vec);
void VecAdd(float * mResult, float * vec1, float * vec2);
void DrawLine(Camera cam, Point pos1, Point pos2);
void D3toD2(float * mReturn, float * point, Camera cam);
int dirrection(float * vec1, float * vec2);
Point ProjectPoint2Plane(float * planeNormal, Point pointOnPlane, Point coor);
Point ProjectPoint2Vec(float * PrimVector, float * perpVector, Point pointOnVector, Point coor);
float WorkOutKForProjectVectors(float Bu, float Ax, float Au, float Bx, float Ys, float Yp, float Xs, float Xp);
void Close();
//=========

int main(int argc, char * args[])
{
	Init();

	SDL_Event e;

	Camera cam;

	cam.dirrection.start.x = -10;
	cam.dirrection.start.y = 0;
	cam.dirrection.start.z = 0;

	cam.dirrection.end.x = 0;
	cam.dirrection.end.y = 0;
	cam.dirrection.end.z = 0;

	float yawAng = 0.0;
	float pitchAng = 0.0;

	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					//pitchAng += 0.1;
					//cam.Pitch(pitchAng);
					break;

				case SDLK_RIGHT:
					yawAng += 0.1;
					cam.Yaw(yawAng);
					break;

				case SDLK_DOWN:
					//pitchAng -= 0.1;
					//cam.Pitch(pitchAng);
					break;
				case SDLK_LEFT:
					yawAng -= 0.1;
					cam.Yaw(yawAng);

				default:
					break;
				}
			}

		}

		//proccessing
		//-----------

		//===========


		//render
		//clear screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		Point pos1;
		pos1.x = -1.4142;
		pos1.y = -1.0;
		pos1.z = 0;

		Point pos2;
		pos2.x = 0;
		pos2.y = -1;
		pos2.z = -1.4142;
		DrawLine(cam, pos1, pos2);


		pos2.x = 0;
		pos2.y = -1;
		pos2.z = 1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 0;
		pos2.y = 1;
		pos2.z = 1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = -1.4142;
		pos2.y = 1;
		pos2.z = 0;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 0;
		pos2.y = 1;
		pos2.z = -1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 0;
		pos2.y = -1;
		pos2.z = -1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 1.4142;
		pos2.y = -1;
		pos2.z = 0;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 0;
		pos2.y = -1;
		pos2.z = -1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 0;
		pos2.y = 1;
		pos2.z = -1.4142;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 1.4142;
		pos2.y = 1;
		pos2.z = 0;
		DrawLine(cam, pos1, pos2);

		pos1 = pos2;

		pos2.x = 1.4142;
		pos2.y = -1;
		pos2.z = 0;
		DrawLine(cam, pos1, pos2);

		SDL_RenderPresent(gRenderer);

	}
	Close();
	return 0;
}

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		std::cout << "can't initialize SDL video. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gWindow = SDL_CreateWindow("Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!gWindow)
	{
		std::cout << "can't create a window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!gRenderer)
	{
		std::cout << "can't create a renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

void DrawLine(Camera cam , Point pos1, Point pos2)
{
	float pos1F[3];
	pos1F[0] = pos1.x;
	pos1F[1] = pos1.y;
	pos1F[2] = pos1.z;

	float Dpos1[2];
	D3toD2(Dpos1, pos1F, cam);

	float pos2F[3];
	pos2F[0] = pos2.x;
	pos2F[1] = pos2.y;
	pos2F[2] = pos2.z;

	float Dpos2[2];
	D3toD2(Dpos2, pos2F, cam);

	SDL_RenderDrawLine(gRenderer, (SCREEN_WIDTH / 2) + (Dpos1[0] * M2P), (SCREEN_HEIGHT / 2) + (Dpos1[1] * M2P), (SCREEN_WIDTH / 2) + (Dpos2[0] * M2P), (SCREEN_HEIGHT / 2) + (Dpos2[1] * M2P));
}

void D3toD2(float * mReturn, float * point, Camera cam)
{
	//workout camera vector
	float camVec[3];
	cam.dirrection.GetSlope(camVec);

	//convert coordant to a point
	Point target;
	target.x = point[0];
	target.y = point[1];
	target.z = point[2];
	
	//project the target onto the plane
	Point projection = ProjectPoint2Plane(camVec, cam.dirrection.start, target);

	//work out y vector
	float UnitYVec[3];
	cam.up.GetUnitVec(UnitYVec);
	//std::cout << "y vector = " << UnitYVec[0] << " " << UnitYVec[1] << " " << UnitYVec[2] << " " << std::endl;


	//workout x vector
	float UnitXVec[3];
	float UnitCamVec[3];
	cam.dirrection.GetUnitVec(UnitCamVec);
	CrossProduct(UnitXVec, UnitYVec, UnitCamVec);
	//std::cout << "x vector = " << UnitXVec[0] << " " << UnitXVec[1] << " " << UnitXVec[2] << " " << std::endl;

	//find distance along x axis
	Point xAxis = ProjectPoint2Vec(UnitXVec, UnitYVec, cam.dirrection.start, projection);
	Vector xDirrection;
	xDirrection.start = cam.dirrection.start;
	xDirrection.end = xAxis;
	float xDir = xDirrection.GetLength();

	//find which dirrection
	float pointVec[3];
	xDirrection.GetSlope(pointVec);
	xDir = xDir * dirrection(UnitXVec, pointVec);

	//find distance along y axis
	Point yAxis = ProjectPoint2Vec(UnitYVec, UnitXVec, cam.dirrection.start, projection);
	Vector yDirrection;
	yDirrection.start = cam.dirrection.start;
	yDirrection.end = yAxis;
	float yDir = yDirrection.GetLength();

	//find which dirrection
	yDirrection.GetSlope(pointVec);
	yDir = yDir * dirrection(UnitYVec, pointVec);


	//return answer
	mReturn[0] = xDir;
	mReturn[1] = yDir;
}

float DotProduct(float * vec1, float * vec2)
{
	float sum = 0;

	for (int i = 0; i < 3; i++)
	{
		sum += vec1[i] * vec2[i];
	}
	return sum;
}

void CrossProduct(float * mReturn, float * vec1, float * vec2)
{
	//hardcode it for 3D
	mReturn[0] = (vec1[1] * vec2[2]) - (vec1[2] * vec2[1]);
	mReturn[1] = (vec1[2] * vec2[0]) - (vec1[0] * vec2[2]);
	mReturn[2] = (vec1[0] * vec2[1]) - (vec1[1] * vec2[0]);
}

void ScaleProduct(float scale, float * vec)
{
	for (int i = 0; i < 3; i++)
	{
		vec[i] = scale * vec[i];
	}
}

void VecAdd(float * mResult, float * vec1, float * vec2)
{
	for (int i = 0; i < 3; i++)
	{
		mResult[i] = vec1[i] + vec2[i];
	}
}

int dirrection(float * vec1, float * vec2)
{
	//work out if negative or positive
	int dirrection = 0;
	int i = 0;
	while ((!dirrection) && i < 3)
	{
		if (vec1[i] * vec2[i] > 0)
		{
			dirrection = 1;
		}
		else if (vec1[i] * vec2[i] < 0)
		{
			dirrection = -1;
		}
		i++;
		if (!(i < 3))
		{
			//std::cout << "looping too many times for working out dirrection\n";
		}
	}
	return dirrection;
}

Point ProjectPoint2Plane(float * planeNormal, Point pointOnPlane, Point coor)
{	
	//first work out D
	float D = planeNormal[0] * pointOnPlane.x + planeNormal[1] * pointOnPlane.y + planeNormal[2] * pointOnPlane.z;

	//work out n
	float numerator = D - planeNormal[0] * coor.x - planeNormal[1] * coor.y - planeNormal[2] * coor.z;
	float denominator = pow(planeNormal[0], 2) + pow(planeNormal[1], 2) + pow(planeNormal[2], 2);
	float n = numerator / denominator;
	
	//work out the coordiate
	Point projection;
	projection.x = coor.x + n * planeNormal[0];
	projection.y = coor.y + n * planeNormal[1];
	projection.z = coor.z + n * planeNormal[2];

	return projection;
}

Point ProjectPoint2Vec(float * PrimVector, float * perpVector , Point pointOnVector, Point coor)
{
	float k = 0.0;
	//check if vectors r perpendicular--------------------
	if (perpVector[1] * PrimVector[0] != perpVector[0] * PrimVector[1])
	{
		k = WorkOutKForProjectVectors(perpVector[1], PrimVector[0], perpVector[0], PrimVector[1], pointOnVector.y, coor.y, pointOnVector.x, coor.x);
	}
	else if (perpVector[2] * PrimVector[1] != perpVector[1] * PrimVector[2])
	{
		k = WorkOutKForProjectVectors(perpVector[2], PrimVector[1], perpVector[1], PrimVector[2], pointOnVector.z, coor.z, pointOnVector.y, coor.y);
	}
	else if (perpVector[0] * PrimVector[2] != perpVector[2] * PrimVector[0])
	{
		k = WorkOutKForProjectVectors(perpVector[0], PrimVector[2], perpVector[2], PrimVector[0], pointOnVector.x, coor.x, pointOnVector.z, coor.z);
	}
	else
	{
		std::cout << "don't intercept\n";
	}

	Point newCoor;
	newCoor.x = pointOnVector.x + k * PrimVector[0];
	newCoor.y = pointOnVector.y + k * PrimVector[1];
	newCoor.z = pointOnVector.z + k * PrimVector[2];

	return newCoor;
}

float WorkOutKForProjectVectors(float Bu, float Ax, float Au, float Bx, float Ys, float Yp, float Xs, float Xp)
{
	float numeratior = (Au * (Ys - Yp)) - (Bu * (Xs - Xp));
	float denomiatior = (Bu * Ax) - (Au * Bx);

	return numeratior / denomiatior;
}

void Close()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_Quit();
}