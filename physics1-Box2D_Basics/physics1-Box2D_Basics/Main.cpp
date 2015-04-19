#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <Box2D/Box2D.h>

//global variables
//----------------
SDL_Renderer * gRenderer = NULL;
//================
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

class LTimer
{
	Uint32 startTicks;
	Uint32 pausedTicks;
	bool started;
	bool paused;

public:
	LTimer();
	void Start();
	void Reset();
	Uint32 getTicks();
	void Pause();
	void Unpause();
	
	bool isPaused();
	bool isStarted();
};

LTimer::LTimer()
{
	startTicks = 0;
	pausedTicks = 0;
	started = false;
	paused = false;
}

void LTimer::Start()
{
	started = true;
	paused = false;

	//reset timer
	startTicks = SDL_GetTicks();
}

void LTimer::Reset()
{
	started = false;
}

void LTimer::Pause()
{
	//only can pause if has started counting and not paused
	if (started && ! paused)
	{
		paused = true;
		pausedTicks = SDL_GetTicks() - startTicks;
	}
}

void LTimer::Unpause()
{
	if (paused)
	{
		paused = false;
		startTicks = SDL_GetTicks() - pausedTicks;
	}
}

bool LTimer::isPaused()
{
	return paused;
}

bool LTimer::isStarted()
{
	return started;
}

Uint32 LTimer::getTicks()
{
	Uint32 time = 0;

	if (started)
	{
		if (!paused)
		{
			time = SDL_GetTicks() - startTicks;
		}
		else
		{
			time = pausedTicks;
		}
	}

	return time;
}


//global variables
//----------------
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 600;
const int fps = 60;
const float32 ClicksPerFrame = (float32)(1000.0 / fps);
const float M2P = 30;
const float P2M = 1 / M2P;

SDL_Window * gWindow = NULL;
b2World * world = NULL;
LTexture boxTexture;
//================


b2Body * addRect(float32 x, float32 y, float32 w, float32 h, bool dyn = true)
{
	//bodydef like the position and type
	b2BodyDef bodydef;
	bodydef.position.Set(x * P2M, y * P2M);

	if (dyn)
	{
		bodydef.type = b2_dynamicBody;
	}
	else
	{
		bodydef.type = b2_staticBody;
	}

	//the actual body
	b2Body * body = world->CreateBody(&bodydef);

	b2PolygonShape square;
	square.SetAsBox(P2M *w / 2, P2M * h / 2);

	//fixtureDef
	b2FixtureDef fixturedef;
	fixturedef.shape = &square;
	fixturedef.density = 1.0;

	body->CreateFixture(&fixturedef);

	return body;

}

void drawSquare(b2Vec2* points, b2Vec2 center, float angle)
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);

	angle = -angle;

	/*
	points[0].x * cos(angle) + points[0].y * sin(angle)

	-points[0].x * sin(angle) + points[0].y * cos(angle)
	*/

	for (int i = 0; i < 4; i++)
	{
		int j = i + 1;
		if (j > 3)
		{
			j = 0;
		}
		SDL_RenderDrawLine(gRenderer, (center.x + (points[i].x * cos(angle) + points[i].y * sin(angle))) * M2P, (center.y + (-points[i].x * sin(angle) + points[i].y * cos(angle))) * M2P, (center.x + (points[j].x * cos(angle) + points[j].y * sin(angle))) * M2P, (center.y + (-points[j].x * sin(angle) + points[j].y * cos(angle))) * M2P);
	}


	/*
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_Rect fillRect = { center.x * M2P, center.y * M2P, 20, 20 };
	SDL_RenderFillRect(gRenderer, &fillRect);
	*/
}


bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "can't open SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gWindow = SDL_CreateWindow("Physics 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!gWindow)
	{
		std::cout << "can't create a window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	if (!gRenderer)
	{
		std::cout << "can't create renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	b2Vec2 gravity(0.0f, 9.81f);

	world = new b2World(gravity);

	addRect(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 30, false);

	return true;
}

bool loadMedia()
{
	if (!boxTexture.LoadFromFile("res/box.png"))
	{
		std::cout << "can't load texture\n";
		return false;
	}
	return true;
}

void display()
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	b2Body* tmp = world->GetBodyList();
	b2Vec2 points[4];
	while (tmp != NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			points[i] = ((b2PolygonShape*)tmp->GetFixtureList()->GetShape())->GetVertex(i);
		}

		b2Vec2 centrePoint = tmp->GetWorldCenter();

		float angle = tmp->GetAngle();


		drawSquare(points, centrePoint, angle);

		tmp = tmp->GetNext();
	}
	SDL_RenderPresent(gRenderer);
}

void close()
{
	boxTexture.Free();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!init())
	{
		std::cout << "can't initialize\n";
	}

	if (!loadMedia())
	{
		std::cout << "can not load media\n";
	}

	Uint32 frameTime;
	LTimer frameTimer;

	SDL_Event e;

	bool quit = false;
	while (!quit)
	{
		frameTimer.Start();
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
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				std::cout << "created a new square\n";
				int x;
				int y;
				SDL_GetMouseState(&x, &y);
				addRect(x, y, 30, 30, true);
				break;
			}
		}

		//update physics engine
		//to increase persision, bump up the 5, 5
		world->Step(ClicksPerFrame / (float32)1000.0, 8, 3);

		display();

		frameTime = frameTimer.getTicks();
		if (frameTime < ClicksPerFrame)
		{
			SDL_Delay((Uint32)ClicksPerFrame - frameTime);
		}
	}

	close();
	return 0;
}