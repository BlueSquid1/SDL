#include <SDL.h>
#include <iostream>

#include "Matrix.h"

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);

	Matrix A(4,4);
	A.grid[0][0] = 1;
	A.grid[0][1] = 0;
	A.grid[0][2] = 0;
	A.grid[0][3] = 1.5;

	A.grid[1][0] = 0;
	A.grid[1][1] = 1;
	A.grid[1][2] = 0;
	A.grid[1][3] = 1;

	A.grid[2][0] = 0;
	A.grid[2][1] = 0;
	A.grid[2][2] = 1;
	A.grid[2][3] = 1.5;

	A.grid[3][0] = 0;
	A.grid[3][1] = 0;
	A.grid[3][2] = 0;
	A.grid[3][3] = 1;

	Matrix B(4, 4);
	B.grid[0][0] = 1;
	B.grid[0][1] = 0;
	B.grid[0][2] = 0;
	B.grid[0][3] = 0;

	B.grid[1][0] = 0;
	B.grid[1][1] = -1;
	B.grid[1][2] = 0;
	B.grid[1][3] = 0;

	B.grid[2][0] = 0;
	B.grid[2][1] = 0;
	B.grid[2][2] = -1;
	B.grid[2][3] = 0;

	B.grid[3][0] = 0;
	B.grid[3][1] = 0;
	B.grid[3][2] = 0;
	B.grid[3][3] = 1;

	Matrix C = A * B;

	C.Print();

	SDL_Delay(1000 * 20);

	SDL_Quit();
	return 0;
}