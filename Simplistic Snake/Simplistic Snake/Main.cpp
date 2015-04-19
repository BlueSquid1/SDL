#include <iostream> //for console printouts

#include <SDL.h> //for graphics
#include <SDL_ttf.h> // so can display rendered text onto the screen

//because c++ lets you define multiple functions with the same name
//as a consiquency u need to tell the compilar which to use before
//you refer to the function in your main code

//functions
//---------
bool Init(); // there will be a function called Init and it will return a bool
void Close(); // void means nothing will be returned
//=========

//if u want to use global variable then u need to declare them here:
//global variables
//----------------
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window * gWindow = NULL; //pointer
SDL_Renderer * gRenderer = NULL; //pointer
//================

//entry point
int main(int argc, char* args[])
{
	bool wasSuccesful;

	//first we need to initalize everything
	wasSuccesful= Init(); //runs the function Init()
	if (wasSuccesful == false)
	{
		std::cout << "can't initalize :(" << std::endl;
	}

	//variables that aren't global can go here.
	//they will be deleted at the end of main
	//--------------
	SDL_Event e; //stores all the events that happen in each frame
	//remember we are doing event based programing

	int frameCounter = 0; // counts the frames

	int dirrection = 0; //dirrection the snake is heading
	/*
	lets make:
	0 = up
	1 = Right
	2 = Down
	3 = Left
	*/
	
	int snakePosX = 304; // this is where the snake will start
	int snakePosY = 200; // in pixels

	int snakeWidth = 8; //the snake will be a box that is 8 by 8 pixels
	//==============

	//then we need to start the main loop
	bool quit = false; //set this to true will exit out of the loop
	while (quit == false)
	{
		//first need to get user input
		//----------------------------

		//might be more than one input in this frame.
		//so we will loop through all the user inputs until
		//we have check it user input
		while (SDL_PollEvent(&e))
		{
			//the "&" means pass the address of the variable e.
			//most things in SDL use pointers because its faster

			//check if the user is trying to hit the exit button
			if (e.type == SDL_QUIT)
			{
				//the "." means get a property of the variable e
				//the "." is quite hard to understand if your a noob
				//your learn about it in more detail at uni

				//set quit to true which will exit out of the main loop
				quit = true;
			}

			//check if its a button press
			if (e.type == SDL_KEYDOWN)
			{
				//see which button is being pressed
				if (e.key.keysym.sym == SDLK_UP)
				{
					dirrection = 0;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					dirrection = 1;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					dirrection = 2;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					dirrection = 3;
				}
			}

		}
		//=======================

		//proccessing (does stuff like check for collisions and update the position)
		//-----------

		//we don't want to update the position 60 times a second so we will use an
		//if statement to skip every 5 frames

		if (frameCounter >= 5)
		{
			frameCounter = 0; //reset frame counter
			
			//we want to make a backup of the current position of the snake
			//so if the snake is moving off the screen then we can roll back
			int origX = snakePosX;
			int origY = snakePosY;

			//update the position
			if (dirrection == 0) //up
			{
				snakePosY = snakePosY - snakeWidth; // in the SDL world the origin is on the top left corner that's why its a negative
			}
			else if (dirrection == 1) //right
			{
				snakePosX = snakePosX + snakeWidth;
			}
			else if (dirrection == 2) //down
			{
				snakePosY = snakePosY + snakeWidth;
			}
			else if (dirrection == 3) //left
			{
				snakePosX = snakePosX - snakeWidth;
			}

			//check if snake is moving off the screen
			//first lets check in the x dirrection
			if (snakePosX < 0 || snakePosX + snakeWidth > SCREEN_WIDTH)
			{
				//snake is falling off the screen!

				snakePosX = origX; //reset the position
			}

			if (snakePosY < 0 || snakePosY + snakeWidth > SCREEN_HEIGHT)
			{
				//snake is falling off the screen!

				snakePosY = origY; //reset the position
			}
		}
		//===========

		//display
		//-------
		//render snake
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF); //will now start to print in blue

		//now we need to tell SDL which pixels should be the snake
		// the easiest way to do this is to create a variable:
		SDL_Rect snakeHead;
		//it is a rectangle and therefore has the properties height, width and pos
		snakeHead.h = snakeWidth; //height (in pixels)
		snakeHead.w = snakeWidth;// width (in pixels)
		snakeHead.x = snakePosX; // x pos (in pixels)
		snakeHead.y = snakePosY; // y pos (in pixels)

		//now all we do is tell SDL to colour in the box
		SDL_RenderFillRect(gRenderer, &snakeHead);

		//up until now we have been writing our graphics to VRAM (Video RAM). we do this because it is fast.
		//the next line of code tells it to move everything from VRAM to the display port (which is what u see)
		SDL_RenderPresent(gRenderer);
		//=======

		frameCounter = frameCounter + 1; // add 1 to the frameCounter
	}

	Close(); //when done close everything which I created safely
	return 0;
}

//returns a boolean value (true or false)
bool Init()
{
	//true means the function ran sucsessfully
	//false means something went wrong

	SDL_Init(SDL_INIT_VIDEO); //tell SDL that u want to initalize video tools

	//create a window
	gWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (gWindow == NULL)
	{
		std::cout << "failed to create a window\n";
		return false;
	}
	else
	{
		//we need to tell SDL that you want to render with your graphics card
		//you do this by setting up a renderer
		//a renderer is able to take a picture and tell your hardware what to do with it
		gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		//the -1 means it will scan your hardware until it finds a hardware interface that can do
		//SDL_RENDERER_ACCELERATED and SDL_RENDERER_PRESENTVSYNC

		if (gRenderer == NULL)
		{
			std::cout << "can't create a renderer. you computer might not have the right hardware\n";
			return false;
		}
		else
		{
			return true;
		}
	}

	//should never get here but lets say it failed it if ever did
	return false;
}

void Close()
{
	//c++ clears memory in a really crappy way
	//it delete pointers
	//so if u make a window and make a variable point to it. c++ will delete the pointer
	//but the actual window will not be deleted. 
	//This is crap because without the pointer c++ has no idea where the window is stored.
	
	SDL_DestroyWindow(gWindow); // destroys the window but does not destroy the pointer
	//c++ will automatically delete the pointer at the end of the function.

	//interesting fact c++ doesn't actually delete things on your hard drive. it just tells itself
	//that it no longer exists. its like tony abbort and his view on global warming.

	SDL_DestroyRenderer(gRenderer); //destroys the renderer

	SDL_Quit(); //close SDL
}