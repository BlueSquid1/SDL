#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL.h>
#define PROGRAM_NAME "Tutorial1"

void sdldie(const char *msg)
{
	printf("%s:%s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(1);
}

void checkSDLError(int line = -1)
{
#ifndef NDEBUG
	const char * error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
		{
			printf(" + line: %i\n", line);
		}
		SDL_ClearError();
	}
#endif
}

int main(int argc, char * argv[])
{
	SDL_Window * mainwindow;
	SDL_GLContext maincontext;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		sdldie("Unable to initialize SDL");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mainwindow = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (!mainwindow)
	{
		sdldie("unable to create window");
	}

	checkSDLError(__LINE__);

	maincontext = SDL_GL_CreateContext(mainwindow);

	checkSDLError(__LINE__);

	SDL_GL_SetSwapInterval(1);

	glClearColor(1.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(mainwindow);

	SDL_Delay(2000);

	glClearColor(0.0, 1.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(mainwindow);

	SDL_Delay(2000);


	glClearColor(0.0, 0.0, 1.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(mainwindow);

	SDL_Delay(2000);

	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();

	return 0;
}