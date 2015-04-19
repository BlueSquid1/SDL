#include <SDL.h>
#include <SDL_image.h>
#include <iostream>



//global variables----
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
const int WALKING_ANIMATION_FRAMES = 7;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
//====================

class LTexture
{
	SDL_Texture * mTexture;
	int mWidth;
	int mHeight;

public:
	LTexture();
	~LTexture();

	bool LoadFromFile(std::string path);
	void Free();

	//give texture a ting colour
	void SetColor(Uint8 red, Uint8 green, Uint8 blue);

	void SetBlendMode(SDL_BlendMode blending);
	void SetAlpha(Uint8 alpha);

	void Render(int x, int y, SDL_Rect* clip = NULL);

	//mutaic functions
	//----------------
	int GetWidth();
	int GetHeight();
	//================
};

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	this->Free();
}

bool LTexture::LoadFromFile(std::string path)
{
	this->Free();

	SDL_Texture * newTexture = NULL;

	SDL_Surface * gLoadedTexture = NULL;

	gLoadedTexture = IMG_Load(path.c_str());
	if (!gLoadedTexture)
	{
		std::cout << "can't load: " << path.c_str() << " check the file exists. IMG error: " << IMG_GetError() << std::endl;
		return false;
	}

	//set colour color key
	SDL_SetColorKey(gLoadedTexture, SDL_TRUE, SDL_MapRGB(gLoadedTexture->format, 0x00, 0xFF, 0xFF));

	newTexture = SDL_CreateTextureFromSurface(gRenderer, gLoadedTexture);
	if (!newTexture)
	{
		std::cout << "can't create a texture from the image: " << path.c_str() << " SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	mWidth = gLoadedTexture->w;
	mHeight = gLoadedTexture->h;

	SDL_FreeSurface(gLoadedTexture);

	mTexture = newTexture;
	return true;
}

void LTexture::Free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::SetColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::SetBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::SetAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::Render(int x, int y, SDL_Rect* clip) //clip = square on texture
{
	//make a square on the area that the image is going to be rendered to
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; //where to render on window

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::GetWidth()
{
	return mWidth;
}

int LTexture::GetHeight()
{
	return mHeight;
}

//global variables-----------
LTexture gSpriteSheetTexture;
//===========================

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

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!gRenderer)
	{
		std::cout << "can't create a renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	//set default colour
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
	if (!gSpriteSheetTexture.LoadFromFile("res/charactor_sheet.png"))
	{
		std::cout << "Failed to load image\n";
		return false;
	}
	
	int i = 0;
	int width = 130;
	int height = 155;

	for (i = 0; i < WALKING_ANIMATION_FRAMES; i++)
	{
		gSpriteClips[i].x = i * width;
		gSpriteClips[i].y = 0 * height;
		gSpriteClips[i].w = width;
		gSpriteClips[i].h = height;
	}

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

	int frame = 0;

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
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		gSpriteSheetTexture.Render(0, 0, &gSpriteClips[frame / 4]);

		//update screen
		SDL_RenderPresent(gRenderer);

		frame++;

		if (frame / 4 >= WALKING_ANIMATION_FRAMES)
		{
			frame = 0;
		}
	}

	Close();
	return 0;
}