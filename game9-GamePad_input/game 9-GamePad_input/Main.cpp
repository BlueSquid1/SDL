#include <SDL.h>
#include <SDL_image.h> //for importing image formats
#include <iostream> //console outputs
#include <cmath>



//global variables----
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
const int JOYSTICK_DEAD_ZONE = 8000;
SDL_Joystick * gGameController = NULL;
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

#ifdef _SDL_TTF_H
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

	void Free();

	//give texture a ting colour
	void SetColor(Uint8 red, Uint8 green, Uint8 blue);

	void SetBlendMode(SDL_BlendMode blending);
	void SetAlpha(Uint8 alpha);

	void Render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point * center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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

	SDL_Surface * gLoadedSurface = NULL;

	gLoadedSurface = IMG_Load(path.c_str());
	if (!gLoadedSurface)
	{
		std::cout << "can't load: " << path.c_str() << " check the file exists. IMG error: " << IMG_GetError() << std::endl;
		return false;
	}

	//set colour color key
	SDL_SetColorKey(gLoadedSurface, SDL_TRUE, SDL_MapRGB(gLoadedSurface->format, 0x00, 0xFF, 0xFF));

	newTexture = SDL_CreateTextureFromSurface(gRenderer, gLoadedSurface);
	if (!newTexture)
	{
		std::cout << "can't create a texture from the image: " << path.c_str() << " SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	mWidth = gLoadedSurface->w;
	mHeight = gLoadedSurface->h;

	SDL_FreeSurface(gLoadedSurface);

	mTexture = newTexture;
	return true;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	Free();
	SDL_Surface * loadedSurface = NULL;
	loadedSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (!loadedSurface)
	{
		std::cout << "can not load text: " << textureText << " TTF error: " << TTF_GetError() << std::endl;
		return false;
	}
	SDL_Texture * newTexture = NULL;
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (!newTexture)
	{
		std::cout << "can't make a texture from text: " << textureText << " SDL error: " << SDL_GetError() << std::endl;
		return false;
	}
	mWidth = loadedSurface->w;
	mHeight = loadedSurface->h;

	mTexture = newTexture;

	SDL_FreeSurface(loadedSurface);
	return true;
}
#endif

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

void LTexture::Render(int x, int y, SDL_Rect* clip, double angle, SDL_Point * center, SDL_RendererFlip flip) //clip = square on texture
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; //where to render on window

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::GetWidth()
{
	return mWidth;
}

int LTexture::GetHeight()
{
	return mHeight;
}

//----global variables-----
LTexture gArrow;
//=========================

//functions----
bool Init();
bool LoadMedia();
void Close();
//=============

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cout << "failed to open SDL video or SDL gameController. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		std::cout << "Warning: linear texture filtering not enabled\n";
	}

	if (SDL_NumJoysticks() < 1)
	{
		std::cout << "Can't find any gameControllers connected\n";
	}
	else
	{
		gGameController = SDL_JoystickOpen(0);
		if (!gGameController)
		{
			std::cout << "failed to initizalize game controller. SDL error: " << SDL_GetError() << std::endl;
			return false;
		}
	}

	gWindow = SDL_CreateWindow("Mouse clicks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
	gArrow.LoadFromFile("res/arrow.png");
	return true;
}

void Close()
{
	gArrow.Free();

	SDL_JoystickClose(gGameController);
	gGameController = NULL;

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

	//remembers which dirrection the controller is pointing
	int xDir = 0;
	int yDir = 0;

	double joystickAngle;

	bool quit = false;
	while (!quit)
	{
		//updates keystates
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type = SDL_JOYAXISMOTION)
			{
				//motion from controller 0
				if (e.jaxis.which == 0)
				{
					//X axis motion
					if (e.jaxis.axis == 0)
					{
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							xDir = -1;
						}
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							xDir = 1;
						}
						else
						{
							xDir = 0;
						}
					}
					//y axis motion
					else if (e.jaxis.axis == 1)
					{
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							yDir = -1;
						}
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							yDir = 1;
						}
						else
						{
							yDir = 0;
						}
					}
				}
			}
		}

		//clear the screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		joystickAngle = atan2((double)yDir, (double)xDir);

		//convert to degrees
		joystickAngle = joystickAngle * (180 / M_PI);

		if (xDir == 0 && yDir == 0)
		{
			joystickAngle = 0.0;
		}

		gArrow.Render((SCREEN_WIDTH - gArrow.GetWidth()) / 2, (SCREEN_HEIGHT - gArrow.GetHeight()) / 2, NULL, joystickAngle + 90.0, NULL);

		//update screen
		SDL_RenderPresent(gRenderer);
	}

	Close();
	return 0;
}