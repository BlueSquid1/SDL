#include <SDL.h>
#include <SDL_image.h> //for importing image formats
#include <iostream> //console outputs



//global variables----
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;
SDL_Rect gSpriteClips[TOTAL_BUTTONS];
//====================

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT,
	BUTTON_SPRITE_MOUSE_OVER_MOTION,
	BUTTON_SPRITE_MOUSE_DOWN,
	BUTTON_SPRITE_MOUSE_UP,
	BUTTON_SPRITE_MOUSE_TOTAL,
};

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


class LButton
{
	//store topleft corner
	SDL_Rect mPosition;
	//the current state the buttion is in
	LButtonSprite mCurrentSprite;
public:
	//constructor
	LButton();

	//set top left position
	void SetPosition(int x, int y, int w, int h);

	//handles mouse event
	void HandleEvent(SDL_Event* e);

	//shows button
	void Render();
};

LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;
	mPosition.w = 0;
	mPosition.h = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::SetPosition(int x, int y, int w, int h)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.w = w;
	mPosition.h = h;
}

void LButton::HandleEvent(SDL_Event * e)
{
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y); //----------------------------------------------------------------*

		//find if inside the buttion
		bool inside = true;

		if (x < mPosition.x)
		{
			inside = false;
		}

		else if (x > mPosition.x + mPosition.w)
		{
			inside = false;
		}
		else if (y < mPosition.y)
		{
			inside = false;
		}
		else if (y > mPosition.y + mPosition.h)
		{
			inside = false;
		}

		if (inside == false)
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		else
		{
			switch (e->type)
			{
			case SDL_MOUSEMOTION:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;
			case SDL_MOUSEBUTTONUP:
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			default:
				break;
			}
		}
	}
}

LTexture gButtonSpriteSheetTexture;

void LButton::Render()
{
	gButtonSpriteSheetTexture.Render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}


//global variables-----------
LButton gButton;
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

	SDL_Color textColor = { 0, 0, 0 };
	if (!gButtonSpriteSheetTexture.LoadFromFile("res/button.png"))
	{
		std::cout << "Failed to load main texture\n";
		return false;
	}

	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].y = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].w = 256;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OUT].h = 80;

	gSpriteClips[BUTTON_SPRITE_MOUSE_DOWN].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_DOWN].y = 80;
	gSpriteClips[BUTTON_SPRITE_MOUSE_DOWN].w = 256;
	gSpriteClips[BUTTON_SPRITE_MOUSE_DOWN].h = 80;

	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].y = 160;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].w = 256;
	gSpriteClips[BUTTON_SPRITE_MOUSE_OVER_MOTION].h = 80;

	gSpriteClips[BUTTON_SPRITE_MOUSE_UP].x = 0;
	gSpriteClips[BUTTON_SPRITE_MOUSE_UP].y = 240;
	gSpriteClips[BUTTON_SPRITE_MOUSE_UP].w = 256;
	gSpriteClips[BUTTON_SPRITE_MOUSE_UP].h = 80;



	gButton.SetPosition(SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, 256, 80);

	return true;
}

void Close()
{
	gButtonSpriteSheetTexture.Free();

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

	SDL_RendererFlip flipType = SDL_FLIP_NONE;

	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			gButton.HandleEvent(&e);
		}

		//clear the screen
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//render buttions
		gButton.Render();

		//update screen
		SDL_RenderPresent(gRenderer);
	}

	Close();
	return 0;
}