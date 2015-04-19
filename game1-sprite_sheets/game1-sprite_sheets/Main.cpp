#include <SDL.h>
#include <SDL_image.h>
#include <iostream>



SDL_Renderer * gRenderer = NULL;

#include "LTexture.h"


//global variables----
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window * gWindow = NULL;
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;

//====================

//functions----
bool Init();
bool LoadMedia();
void Close();
//=============

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "failed to open SDL video. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		std::cout << "Warning: linear texture filtering not enabled\n";
	}

	gWindow = SDL_CreateWindow("Sprite Cliping", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!gWindow)
	{
		std::cout << "can't create a window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!gRenderer)
	{
		std::cout << "can't create a renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	//set default colour
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	//initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout << "can't initialize SDL_Image. SDL_image error: " << IMG_GetError() << std::endl;
		return false;
	}

	return true;
}

bool LoadMedia()
{
	if (!gSpriteSheetTexture.LoadFromFile("res/sprite.png"))
	{
		std::cout << "Failed to load image\n";
		return false;
	}

	//Set top left sprite
	gSpriteClips[0].x = 0;
	gSpriteClips[0].y = 0;
	gSpriteClips[0].w = 100;
	gSpriteClips[0].h = 100;

	//Set top right sprite
	gSpriteClips[1].x = 100;
	gSpriteClips[1].y = 0;
	gSpriteClips[1].w = 100;
	gSpriteClips[1].h = 100;

	//Set bottom left sprite
	gSpriteClips[2].x = 0;
	gSpriteClips[2].y = 100;
	gSpriteClips[2].w = 100;
	gSpriteClips[2].h = 100;

	//Set bottom right sprite
	gSpriteClips[3].x = 100;
	gSpriteClips[3].y = 100;
	gSpriteClips[3].w = 100;
	gSpriteClips[3].h = 100;

	return true;
}

void Close()
{
	gSpriteSheetTexture.Free();

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!Init())
	{
		std::cout << "failed to initialize\n";
	}

	if (!LoadMedia())
	{
		std::cout << "failed to load media\n";
	}

	SDL_Event e;

	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		//clear the screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		//render top left sprite
		gSpriteSheetTexture.Render(0, 0, &gSpriteClips[0]);

		gSpriteSheetTexture.Render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

		//Render bottom left sprite
		gSpriteSheetTexture.Render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

		//Render bottom right sprite
		gSpriteSheetTexture.Render(SCREEN_WIDTH - gSpriteClips[3].w, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);

		//update screen
		SDL_RenderPresent(gRenderer);
	}

	Close();
	return 0;
}