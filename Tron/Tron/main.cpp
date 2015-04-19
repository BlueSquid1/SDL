#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <cmath>
#include <sstream>

//Global variables
//----------------
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;
TTF_Font * gFont = NULL;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
//================

enum SnakeDirrections
{
	SNAKE_UP,
	SNAKE_RIGHT,
	SNAKE_DOWN,
	SNAKE_LEFT,
	SNAKE_TOTAL_DIRRECTIONS
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
	bool LoadFromRenderedText(std::string textureText, SDL_Color textColor, int size);
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
bool LTexture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, int size)
{
	Free();
	//set font
	TTF_Font * gFont = NULL;
	gFont = TTF_OpenFont("res/impact.ttf", size);
	if (!gFont)
	{
		std::cout << "can't find font\n";
		return false;
	}

	SDL_Surface * loadedSurface = NULL;
	loadedSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (!loadedSurface)
	{
		std::cout << "can not load text: " << textureText << " TTF error: " << TTF_GetError() << std::endl;
		return false;
	}

	TTF_CloseFont(gFont);
	gFont = NULL;

	//set colour color key
	SDL_SetColorKey(loadedSurface, SDL_TRUE, 0);

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
	if (started && !paused)
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

enum buttonstates
{
	BUTTON_OUTOFFOCUS,
	BUTTON_DOWN,
	BUTTON_HOVER
};

class Button
{
	std::string text;
	LTexture textTexture;
	LTexture buttonTexture;
	int butX;
	int butY;
	int textX;
	int textY;

	//for buttion
	//-----------
	int width;
	int height;
	//===========
	int status;
public:
	Button();
	~Button();
	void SetButton(std::string mText, int x, int y, std::string mPath);
	void SetPosition(int x, int y);
	int GetStatus();
	void SetStatus(int mStatus);
	void HandleEvents(SDL_Event &e);
	void Render();
};

Button::Button()
{
	text = "";
	butX = 0;
	butY = 0;
	textX = 0;
	textY = 0;

	status = BUTTON_OUTOFFOCUS;
}

Button::~Button()
{
	textTexture.Free();
	buttonTexture.Free();
}

void Button::SetButton(std::string mText, int x, int y, std::string mPath)
{
	buttonTexture.LoadFromFile(mPath);
	width = buttonTexture.GetWidth();
	height = buttonTexture.GetHeight();

	SDL_Color textColor;
	textColor.a = 255;
	textColor.b = 0;
	textColor.g = 0;
	textColor.r = 0;

	textTexture.LoadFromRenderedText(mText, textColor, 20);

	this->SetPosition(x, y);

}

void Button::SetPosition(int x, int y)
{
	butX = x;
	butY = y;

	textX = butX + (200 - textTexture.GetWidth()) / 2;
	textY = butY + (65 - textTexture.GetHeight()) / 2;
}

int Button::GetStatus()
{
	return status;
}

void Button::SetStatus(int mStatus)
{
	status = mStatus;
}

void Button::HandleEvents(SDL_Event &e)
{
	if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		bool overButton = true;

		if (x < butX)
		{
			overButton = false;
		}
		if (x > butX + width)
		{
			overButton = false;
		}
		if (y < butY)
		{
			overButton = false;
		}
		if (y > butY + 65)
		{
			overButton = false;
		}

		if (overButton == false)
		{
			status = BUTTON_OUTOFFOCUS;
		}
		else
		{
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				status = BUTTON_DOWN;
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				status = BUTTON_HOVER;
			}
		}
	}
}

void Button::Render()
{
	//render button texture
	SDL_Rect buttonClip;
	switch (status)
	{
	case BUTTON_OUTOFFOCUS:
		buttonClip = { 0, 0, 200, 65 };
		break;
	case BUTTON_HOVER:
		buttonClip = { 0, 130, 200, 65 };
		break;
	case BUTTON_DOWN:
		buttonClip = { 0, 65, 200, 65 };
	default:
		break;
	}

	buttonTexture.Render(butX, butY, &buttonClip);
	textTexture.Render(textX, textY);
}



class Snake
{
	int dirrection;
	//to stop the player from making the snake turn 180 degrees between update I've create a tmp variable
	//when the snake is ready to change dirrection it sets dirrection = dirrectionTMP
	int dirrectionTMP; //for changing dirrections

	float speed; //pixels per frame
	int width; //pixels
	int height; //pixels
	int lives;
	SDL_Point position; //pixels, pixels
	LTexture SnakeHead;
	LTexture SnakeTail;
	LTexture life;
	bool grid[SCREEN_HEIGHT / 8][SCREEN_WIDTH / 8];

public:
	Snake();
	void SetSnake(int mX, int mY, int mDirrection, int mLives);
	~Snake();
	int GetDirrection();
	int GetDirrectionTMP();
	void SetDirrection(int mDirrection);
	void SetLives(int mLives);
	int GetLives();
	SDL_Point GetPosition();
	void SetPosition(SDL_Point mPostion);
	float GetSpeed();
	bool LoadTextureHead(std::string path);
	bool LoadTextureTail(std::string path);
	bool LoadTextureLife(std::string path);
	void Render();
	void RenderLives(int x, int y);
	void HandleEvents(SDL_Event & e, int up, int down, int right, int left);
	bool SquareTaken(int x, int y);
	void AddToGrid(int x, int y);
	void ClearGrid();
};

Snake::Snake()
{
	this->SetSnake(0, 0, SNAKE_DOWN, 0);
}

void Snake::SetSnake(int mX, int mY, int mDirrection, int mLives)
{
	this->SetDirrection(mDirrection);
	dirrectionTMP = this->GetDirrection();

	this->SetLives(mLives);

	SDL_Point origin;
	origin.x = mX;
	origin.y = mY;
	this->SetPosition(origin);

	width = 8;
	height = width;
	speed = width;

	this->ClearGrid();

}

Snake::~Snake()
{
	SnakeHead.Free();
	SnakeTail.Free();
	life.Free();
}

int Snake::GetDirrection()
{
	return dirrection;
}

int Snake::GetDirrectionTMP()
{
	return dirrectionTMP;
}

void Snake::SetDirrection(int mDirrection)
{
	if (mDirrection < SNAKE_TOTAL_DIRRECTIONS && mDirrection >= 0)
	{
		dirrection = mDirrection;
	}
}

void Snake::SetLives(int mLives)
{
	lives = mLives;
}
int Snake::GetLives()
{
	return lives;
}

SDL_Point Snake::GetPosition()
{
	return position;
}

void Snake::SetPosition(SDL_Point mPosition)
{
	position.x = mPosition.x;
	position.y = mPosition.y;
}

float Snake::GetSpeed()
{
	return speed;
}

bool Snake::LoadTextureHead(std::string path)
{
	return SnakeHead.LoadFromFile(path);
}

bool Snake::LoadTextureTail(std::string path)
{
	return SnakeTail.LoadFromFile(path);
}

bool Snake::LoadTextureLife(std::string path)
{
	return life.LoadFromFile(path);
}
void Snake::Render()
{
	//render tail
	for (int x = 0; x < SCREEN_WIDTH / 8; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT / 8; y++)
		{
			if (SquareTaken(x, y))
			{
				SnakeTail.Render(x * 8, y * 8);
			}
		}
	}
	//render head
	SnakeHead.Render(GetPosition().x, GetPosition().y);

}

void Snake::RenderLives(int x, int y)
{
	int pos = x;
	for (int i = 0; i < this->GetLives(); i++)
	{
		pos = x + i * 31;
		life.Render(pos, y);
	}
}

void Snake::HandleEvents(SDL_Event & e, int up, int down, int right, int left)
{

	if (e.key.keysym.sym == up)
	{
		if (GetDirrection() != SNAKE_DOWN)
		{
			dirrectionTMP = SNAKE_UP;
		}
	}
	else if (e.key.keysym.sym == down)
	{
		if (GetDirrection() != SNAKE_UP)
		{
			dirrectionTMP = SNAKE_DOWN;
		}
	}
	else if (e.key.keysym.sym == right)
	{
		if (GetDirrection() != SNAKE_LEFT)
		{
			dirrectionTMP = SNAKE_RIGHT;
		}
	}
	else if (e.key.keysym.sym == left)
	{
		if (GetDirrection() != SNAKE_RIGHT)
		{
			dirrectionTMP = SNAKE_LEFT;
		}
	}
}

bool Snake::SquareTaken(int x, int y)
{
	return grid[y][x];
}

void Snake::AddToGrid(int x, int y)
{
	//make sure not overwritting other memory
	if (x >= 0 && x < SCREEN_WIDTH / 8 && y >= 0 && y < SCREEN_HEIGHT / 8)
	{
		grid[y][x] = true;;
	}
}

void Snake::ClearGrid()
{
	for (int i = 0; i < SCREEN_WIDTH / 8; i++)
	{
		for (int j = 0; j < SCREEN_HEIGHT / 8; j++)
		{
			grid[j][i] = false;
		}
	}
}

//Global variables
//----------------
LTexture fpsCounter;
LTexture gBackground;
LTexture gHubBackground;
LTexture gMainMenu;
LTexture Countdown;


int numOfPlayers = 2;
Snake * player = new Snake[numOfPlayers];
//================

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "can't start SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!gWindow)
	{
		std::cout << "can't create window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!gRenderer)
	{
		std::cout << "Can't create renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout << "can't initialize SDL_Image. SDL_image error: " << IMG_GetError() << std::endl;
		return false;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "SDL_TTF could not initialize. TTF error: " << TTF_GetError() << std::endl;
		return false;
	}


	return true;
}

enum EndConditions
{
	M_QUIT,
	M_CONTINUE,
	M_UNDECIDED
};

bool MainMenu(SDL_Event &e)
{
	//load main menu textures

	if (!gMainMenu.LoadFromFile("res/mainMenu.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}
	Button butPlay;
	Button butQuit;
	butPlay.SetButton("Play", 300, 260, "res/button.png");
	butQuit.SetButton("Quit", 300, 400, "res/button.png");

	int result = M_UNDECIDED;
	//repeat until result is assigned to something
	while (!(result != M_UNDECIDED))
	{
		//input
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				result = M_QUIT;
			}
			butPlay.HandleEvents(e);
			butQuit.HandleEvents(e);
		}

		if (butPlay.GetStatus() == BUTTON_DOWN)
		{
			result = M_CONTINUE;
		}
		else if (butQuit.GetStatus() == BUTTON_DOWN)
		{
			result = M_QUIT;
		}

		//clear screen
		SDL_Rect window = { 0, 0, 800, 600 };
		SDL_RenderSetViewport(gRenderer, &window);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//render elements
		gMainMenu.Render(0, 0);

		butPlay.Render();
		butQuit.Render();

		//update screen
		SDL_RenderPresent(gRenderer);
	}

	//unload main menu textures
	gMainMenu.Free();

	if (result == M_CONTINUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LoadMedia()
{
	if (!gBackground.LoadFromFile("res/background.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}

	if (!gHubBackground.LoadFromFile("res/hubBackground.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}

	//for each snake
	if (!player[0].LoadTextureHead("res/snakeHead1.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}

	if (!player[1].LoadTextureHead("res/snakeHead2.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}

	if (!player[0].LoadTextureTail("res/snakeTail1.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}
	if (!player[1].LoadTextureTail("res/snakeTail2.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}

	if (!player[0].LoadTextureLife("res/life1.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}
	if (!player[1].LoadTextureLife("res/life2.png"))
	{
		std::cout << "can't find texture\n";
		return false;
	}
	return true;
}

void Close()
{
	gBackground.Free();
	fpsCounter.Free();
	gHubBackground.Free();
	Countdown.Free();

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
		std::cout << "can't initialize\n";
	}

	bool quit = false;

	SDL_Event e;

	if (!MainMenu(e))
	{
		quit = true;
	}

	if (!LoadMedia())
	{
		std::cout << "can't load media\n";
	}

	//set up the snakes
	player[0].SetSnake(144, 200, SNAKE_RIGHT, 3);
	player[1].SetSnake(656, 200, SNAKE_LEFT, 3);

	std::stringstream ss;
	std::stringstream countDownSS;

	SDL_Color textColour;

	Uint32 framePeriod = 0;
	Uint32 frameCounter = 0;

	Button butRestart;
	butRestart.SetButton("restart", 300, 400, "res/button.png");

	int fps = 0;

	bool started = false;
	bool ended = false;

	LTimer startTimer;
	startTimer.Start();

	LTimer frameTimer;
	frameTimer.Start();
	while (!quit)
	{
		framePeriod = frameTimer.getTicks();
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_p)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				player[0].HandleEvents(e, SDLK_UP, SDLK_DOWN, SDLK_RIGHT, SDLK_LEFT);
				player[1].HandleEvents(e, SDLK_w, SDLK_s, SDLK_d, SDLK_a);
			}
			if (ended == true)
			{
				butRestart.HandleEvents(e);
			}
		}

		//proccessing
		//-----------
		if (frameCounter % 60 == 0)
		{
			if (framePeriod != 0)
			{
				fps = 1000 * 60 / framePeriod;
			}
			else
			{
				fps = 0;
			}
			ss.str("");
			ss << fps;
			frameTimer.Start();
		}

		if (started == false)
		{
			//render a countdown
			countDownSS.str("");
			countDownSS << int(3 - startTimer.getTicks() / 1000);
			if (startTimer.getTicks() >= 3 * 1000)
			{
				started = true;
			}
		}


		if (ended == true)
		{
			if (butRestart.GetStatus() == BUTTON_DOWN)
			{
				started = false;
				ended = false;
				//reset everything
				player[0].SetSnake(144, 200, SNAKE_RIGHT, 3);
				player[1].SetSnake(656, 200, SNAKE_LEFT, 3);
				startTimer.Start();
				butRestart.SetStatus(BUTTON_OUTOFFOCUS);
			}
		}

		if (frameCounter % 3 == 0 && started == true && ended != true)
		{
			for (int i = 0; i < numOfPlayers; i++)
			{
				int newX = player[i].GetPosition().x;
				int newY = player[i].GetPosition().y;

				player[i].SetDirrection(player[i].GetDirrectionTMP());

				switch (player[i].GetDirrection())
				{
				case SNAKE_UP:
					newY -= player[i].GetSpeed();
					break;
				case SNAKE_DOWN:
					newY += player[i].GetSpeed();
					break;
				case SNAKE_RIGHT:
					newX += player[i].GetSpeed();
					break;
				case SNAKE_LEFT:
					newX -= player[i].GetSpeed();
					break;
				default:
					break;
				}
				int coorX = newX / 8;
				int coorY = newY / 8;
				//work out if collision
				bool collision = false;
				for (int j = 0; j < numOfPlayers; j++)
				{
					if (player[j].SquareTaken(coorX, coorY))
					{
						collision = true;
					}
				}
				//check boundarys
				if (coorX < 0 || coorX >= SCREEN_WIDTH / 8 || coorY < 0 || coorY >= 480 / 8)
				{
					collision = true;
				}

				if (collision == true)
				{
					std::cout << "number: " << i << " Died\n";
					player[i].SetLives(player[i].GetLives() - 1);

					if (player[i].GetLives() <= 0)
					{
						ended = true;
					}
					else
					{
						//restart
						started = false;

						//move snakes back
						player[0].SetSnake(144, 200, SNAKE_RIGHT, player[0].GetLives());
						player[1].SetSnake(656, 200, SNAKE_LEFT, player[1].GetLives());
						startTimer.Start();
					}
				}
				else
				{
					player[i].AddToGrid(coorX, coorY);
					SDL_Point newPosition = { newX, newY };
					player[i].SetPosition(newPosition);
				}
			}
		}

		//=============

		//clear screen
		SDL_Rect window = { 0, 0, 800, 600 };
		SDL_RenderSetViewport(gRenderer, &window);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//render background
		SDL_Rect game = { 0, 120, 800, 480 };
		SDL_RenderSetViewport(gRenderer, &game);
		gBackground.Render(0, 0);

		//render snakes
		for (int i = 0; i < numOfPlayers; i++)
		{
			player[i].Render();
		}

		//render the countdown
		if (started == false)
		{
			textColour = { 34, 112, 43 };
			Countdown.LoadFromRenderedText(countDownSS.str().c_str(), textColour, 60);
			Countdown.Render((SCREEN_WIDTH - Countdown.GetWidth()) / 2, (SCREEN_HEIGHT - Countdown.GetWidth()) / 2);

		}

		if (ended == true)
		{
			std::cout << "ended\n";
			countDownSS.str("");
			if (player[0].GetLives() <= 0)
			{
				countDownSS << "Player " << 2 << " wins!";
			}
			else
			{
				countDownSS << "Player " << 1 << " wins!";
			}
			//might as well recycle Countdown variables
			textColour = { 34, 112, 43 };
			Countdown.LoadFromRenderedText(countDownSS.str().c_str(), textColour, 60);
			Countdown.Render((SCREEN_WIDTH - Countdown.GetWidth()) / 2, (SCREEN_HEIGHT - Countdown.GetWidth()) / 2);

			//display button to play again
			SDL_Rect window = { 0, 0, 800, 600 };
			SDL_RenderSetViewport(gRenderer, &window);
			butRestart.Render();
		}

		//render hub
		SDL_Rect hub = { 0, 0, 800, 120 };
		SDL_RenderSetViewport(gRenderer, &hub);
		gHubBackground.Render(0, 0);

		//update lives
		player[0].RenderLives(175, 40);
		player[1].RenderLives(507, 40);

		//render fps
		textColour = { 255, 255, 0 };
		fpsCounter.LoadFromRenderedText(ss.str().c_str(), textColour, 15);
		fpsCounter.Render(5, 5);

		//update screen
		SDL_RenderPresent(gRenderer);

		frameCounter++;
	}

	return 0;
}