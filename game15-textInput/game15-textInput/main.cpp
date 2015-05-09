#include <SDL.h>
#include <SDL_image.h> //for importing image formats
#include <iostream> //console outputs
#include <SDL_ttf.h> //for rendering text to screen

const int WIDTH = 800;
const int HEIGHT = 600;

SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
TTF_Font * gFont = NULL;

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

LTexture gInputTextTexture;

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
	if (textureText.size() <= 0)
	{
		textureText = " ";
	}

	Free();
	SDL_Surface * loadedSurface = NULL;
	loadedSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (!loadedSurface)
	{
		std::cout << "can not load text: " << textureText.c_str() << " TTF error: " << TTF_GetError() << std::endl;
		return false;
	}
	SDL_Texture * newTexture = NULL;
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (!newTexture)
	{
		std::cout << "can't make a texture from text: " << textureText.c_str() << " SDL error: " << SDL_GetError() << std::endl;
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





int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);

	gWindow = SDL_CreateWindow("TextInput", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	IMG_Init(IMG_INIT_PNG);

	TTF_Init();

	gFont = TTF_OpenFont("res/VeraMono.ttf", 15);


	SDL_Event e;

	SDL_Color textColour = { 0x00, 0x00, 0x00, 0xFF };

	std::string inputText;
	int curserPos = 0;

	gInputTextTexture.loadFromRenderedText(inputText, textColour);

	SDL_StartTextInput();

	bool quit = false;
	while (!quit)
	{
		bool updateText = false;

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_LEFT:
					if (curserPos > 0)
					{
						curserPos--;
					}
					break;
				case SDLK_RIGHT:
					if (curserPos < inputText.size())
					{
						curserPos++;
					}
					break;
				case SDLK_BACKSPACE:
				{
					if (curserPos > 0)
					{
						if (inputText.size() == curserPos)
						{
							//deleting from the end of the string
							inputText.pop_back();
							curserPos--;
							updateText = true;
						}
						else
						{
							//deleting from the middle of the string
							inputText.erase(curserPos - 1, 1);
							curserPos--;
							updateText = true;
						}
					}
					break;
				}
				case SDLK_DELETE:
				{
					if (curserPos < inputText.size())
					{
						if (curserPos == inputText.size() - 1)
						{
							//deleting from the end of the string
							inputText.pop_back();
							updateText = true;
						}
						else
						{
							//deleting from the middle of the string
							inputText.erase(curserPos, 1);
							updateText = true;
						}
					}
					break;
				}
				default:
					//ignore key pressed
					break;
				}
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				//add the char at the curser pos
				if (inputText.size() == curserPos)
				{
					//add char to the end of the string
					inputText += e.text.text;
					curserPos++;
				}
				else
				{
					//insert into the middle of the string
					inputText.insert(curserPos, e.text.text);
					curserPos++;
				}
				updateText = true;
			}
		}

		if (updateText)
		{
			if (inputText.size() >= 0)
			{
				gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColour);
			}
		}


		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		int startPosX = WIDTH/3.0;

		int startPosY = HEIGHT / 3.0;

		gInputTextTexture.Render(startPosX, startPosY);

		SDL_Rect curserPic;
		curserPic.x = startPosX + (curserPos) * 9;
		curserPic.y = startPosY;
		curserPic.w = 1;
		curserPic.h = 16;

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &curserPic);

		SDL_RenderPresent(gRenderer);
	}

	SDL_StopTextInput();
	SDL_DestroyWindow(gWindow);
	SDL_DestroyRenderer(gRenderer);
	TTF_CloseFont(gFont);
	SDL_Quit();
	return 0;
}