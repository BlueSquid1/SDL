#include <SDL.h>
#include <SDL_image.h> //for importing image formats
#include <SDL_ttf.h> //for rendering text to screen
#include <string>
#include <cmath>
#include <iostream> //console outputs
#include <SDL_mixer.h>



//global variables----
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
TTF_Font * gFont = NULL;
const int SPRITE_POSITIONS = 7;
SDL_Rect positions[SPRITE_POSITIONS + 1];
SDL_Rect cucPositions[SPRITE_POSITIONS + 1];
Mix_Music *gMusic = NULL;
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

	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

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

//global variables-----------
LTexture textTexture;
LTexture peanutSprite;
LTexture cucSprite;
//===========================

//functions----
bool Init();
bool LoadMedia();
void Close();
//=============

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cout << "failed to open SDL video. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		std::cout << "Warning: linear texture filtering not enabled\n";
	}

	gWindow = SDL_CreateWindow("PEANUTBUTTER JELLY TIME!!!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

	//initialize ttf loading
	if (TTF_Init() == -1)
	{
		std::cout << "SDL_TTF could not initialize. TTF error: " << TTF_GetError() << std::endl;
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	return true;
}

bool LoadMedia()
{
	gFont = TTF_OpenFont("res/impact.ttf", 60);

	if (!gFont)
	{
		std::cout << "can't find: res/impact.ttf TTF error: " << TTF_GetError() << std::endl;
		return false;
	}

	SDL_Color textColor = { 0, 0, 0 };
	if (!textTexture.loadFromRenderedText("Hey there sexy ;)", textColor)) //----------
	{
		std::cout << "Failed to load text\n";
		return false;
	}

	if (!peanutSprite.LoadFromFile("res/peanut_sprite.png"))
	{
		std::cout << "Failed to open image\n";
		return false;
	}

	if (!cucSprite.LoadFromFile("res/peanut_sprite.png"))
	{
		std::cout << "Failed to open image\n";
		return false;
	}

	gMusic = Mix_LoadMUS("res/peanutButter.wav");
	if (!gMusic)
	{
		std::cout << "can't find: res/peanutButter.wav Mix error: " << Mix_GetError << std::endl;
	}

	int i;
	for (i = 0; i <= SPRITE_POSITIONS; i++)
	{
		positions[i].x = i * 28;
		positions[i].y = 0;
		positions[i].w = 28;
		positions[i].h = 35;

		cucPositions[i].x = i * 28;
		cucPositions[i].y = 35;
		cucPositions[i].w = 28;
		cucPositions[i].h = 35;
	}
	return true;
}

void Close()
{
	textTexture.Free();
	peanutSprite.Free();
	cucSprite.Free();

	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	TTF_Quit();
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
	int current = 0;
	int Imain = 0;


	Mix_PlayMusic(gMusic, -1);

	

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

		textTexture.Render((SCREEN_WIDTH - textTexture.GetWidth()) / 2, (SCREEN_HEIGHT - textTexture.GetHeight()) / 2);

		int i;
		
		for (i = 0; i < 15; i++)
		{
			peanutSprite.Render(i * 50 + 20, 200, &positions[current / 10]);
			cucSprite.Render(i * 50 + 20, 130, &cucPositions[current / 10]);
		}

		
		for (i = 0; i < 15; i++)
		{
			peanutSprite.Render(i * 50 + 20, 400, &positions[current / 10]);
			cucSprite.Render(i * 50 + 20, 470, &cucPositions[current / 10]);
		}

		peanutSprite.Render(400, 80, &positions[Imain / 3]);

		Imain++;

		current++;

		//update screen
		SDL_RenderPresent(gRenderer);

		if (current / 10 >= SPRITE_POSITIONS)
		{
			current = 0;
		}

		if (Imain / 2 >= SPRITE_POSITIONS)
		{
			Imain = 0;
		}
	}

	Close();
	return 0;
}