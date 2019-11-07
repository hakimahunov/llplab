#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "efm32gg.h"

//====== Macros and globall variables ======//
// LCD 
#define SIZEX			320	
#define SIZEY			240
#define CENTERX			SIZEX / 2
#define CENTERY			SIZEY / 2
#define SCREENSIZE		SIZEX * SIZEY 
#define FILESIZE		(SCREENSIZE) * sizeof(short) // Size in bytes of the frame buffer
#define RED				0xf800
#define GREEN			0x0400
#define BLUE			0x001f		
#define YELLOW			0xffe0
#define WHITE			0xffff

// Driver 
#define BTN_CHECKER(shift_amount, state) ((1<<(shift_amount)) & ~(state))

// Snake 
#define BORDERTHICKNESS 3
#define SNAKEHEIGHT 	6

// LCD 
int lcdDriver;
short* frameBuffer;
struct fb_copyarea shape;

// Driver 
FILE* dev;
enum Direction {Left = 4, Up = 5, Right = 6, Down = 7};

// Snake 
struct coordinate
{
    int x;
    int y;
};
typedef struct coordinate coordinate;
coordinate snake[500];
coordinate food;
coordinate randcoor;
int snakeDirection; 
int snakeLength;
int foodNumber;
bool gameOverVar = false;

//====== Local functions ======//
void energyOptimizationRoutine();
void initScreen();
void cleanScreen();
void deathScreen();
int setGpadDriver();
void inputHandler(int signal);
void initSnake();
void moveSnake();
void checkBorder();
void checkFood();
void checkSnake();
void gameOver();

int main(int argc, char *argv[])
{
	energyOptimizationRoutine();
    initScreen();
    cleanScreen();
    initSnake();

    int result = setGpadDriver();
    if (result == EXIT_FAILURE) 
	{
		printf("Driver initialization FAILED.");
		exit(EXIT_FAILURE);
	}

	while (1) 
	{
		if(!gameOverVar) // Always move snake
		{
			moveSnake();
		}
		else 			// If game is over, start again
		{
			cleanScreen();
    		initSnake();
    		gameOverVar = false;
		}	
	}	
	
 	exit(EXIT_SUCCESS);
}

//====== Energy optimization ======//
void energyOptimizationRoutine()
{
    *EMU_MEMCTRL = 0x3;			// Disable RAM blocks 1-2
    *MSC_READCTRL |= (1 << 3);	// Disable instruction cache
}

//====== Setup LCD screen ======//
void initScreen()
{
	lcdDriver = open("/dev/fb0", 2);
	frameBuffer = (short*) mmap(NULL, FILESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, lcdDriver, 0);
	
	// Set variables to refresh screen
	shape.dx = 0;
	shape.dy = 0;
	shape.width = SIZEX;
	shape.height = SIZEY;
}

//====== Clean up the screen ======//
void cleanScreen()
{
	int i;
	for (i = 0; i < SCREENSIZE; i++) 
	{
		frameBuffer[i] = 0x0000;
	}
	
	ioctl(lcdDriver, 0x4680, &shape);
}

//====== Print a red screen if game over ======//
void deathScreen()
{
	int i;
	for (i = 0; i < SCREENSIZE; i++) 
	{
		frameBuffer[i] = RED;
	}
	
	ioctl(lcdDriver, 0x4680, &shape);

}

//====== Setup gamepad driver ======//
int setGpadDriver()
{
	dev = fopen("/dev/LLP_GPad_Driver", "rb");
	// (LDD3 Ch.6, p.169)
	if (!dev) {
		printf("Cannot open GPad driver. Try 'modprobe driver-gamepad' command. \n");
		return EXIT_FAILURE;
	}
	
	if (signal(SIGIO, &inputHandler) == SIG_ERR) {
		printf("Error in signal handler registration.\n");
		return EXIT_FAILURE;
	}
	
	if (fcntl(fileno(dev), F_SETOWN, getpid()) == -1) {
		printf("Error in pid setting.\n");
		return EXIT_FAILURE;
	}
	
	long oflags = fcntl(fileno(dev), F_GETFL);
	
	if (fcntl(fileno(dev), F_SETFL, oflags | FASYNC) == -1) {
		printf("Error in FASYNC setting.\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

//====== Take an action when a button is pressed ======//
void inputHandler(int signal)
{
	int input = fgetc(dev);
	
	if (BTN_CHECKER(Left, input)) {
		if (snakeDirection != Right) snakeDirection = Left;
	}
	else if (BTN_CHECKER(Up, input)) {
		if (snakeDirection != Down) snakeDirection = Up;
	}
	else if (BTN_CHECKER(Right, input)) {
		if (snakeDirection != Left) snakeDirection = Right;
	}
	else if (BTN_CHECKER(Down, input)) {
		if (snakeDirection != Up) snakeDirection = Down;
	}
}

//====== Setup snake game ======//
void initSnake()
{
	int a;
	int b;

	snakeLength = 8;
	snakeDirection  = Right;
	foodNumber = 1;
	
	// Create the snake
	for (a = CENTERY; a < CENTERY + SNAKEHEIGHT; a++) 
	{  
		for (b = CENTERX; b < CENTERX + 3 * SNAKEHEIGHT; b++) 
		{ 
			frameBuffer[a * SIZEX + b] = BLUE;
		}
	}
	for (a = 0; a < snakeLength; a++) 
	{
		snake[a].x = CENTERX + a * SNAKEHEIGHT;
		snake[a].y = CENTERY;
	}
	
	// Create the border
	for (a = 0; a < BORDERTHICKNESS; a++) 
	{
		for (b = 0; b < SIZEX; b++) 
		{
			frameBuffer[a * SIZEX + b] = RED;
		}
	}
	for (a = 0; a < SIZEY; a++) 
	{
		for (b = 0; b < BORDERTHICKNESS; b++) 
		{
			frameBuffer[a * SIZEX + b] = RED;
		}
	}		
	for (a = SIZEY - BORDERTHICKNESS; a < SIZEY; a++) 
	{
		for (b = 0; b < SIZEX; b++) 
		{
			frameBuffer[a * SIZEX + b] = RED;
		}
	}
	for (a = 0; a < SIZEY; a++) 
	{
		for (b = SIZEX - BORDERTHICKNESS; b < SIZEX; b++) 
		{
			frameBuffer[a * SIZEX + b] = RED;
		}
	}
	
	// Creating initial food item
	srand(time(NULL));
	randcoor.x = rand() % (319 - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
	randcoor.y = rand() % (239  - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
	for (a = randcoor.y; a < randcoor.y + SNAKEHEIGHT ; a++) 
	{
		for (b = randcoor.x; b < randcoor.x + SNAKEHEIGHT; b++) 
		{
			frameBuffer[a * SIZEX + b] = YELLOW;
		}
	}	
	
	food.x = randcoor.x;
	food.y = randcoor.y;
	ioctl(lcdDriver, 0x4680, &shape);
}	

//====== Move the snake and check if a border, food or the snake itself is touched ======//
void moveSnake()
{
	int i;
	int j;
	int k;
	
	checkBorder();
	
	// Delete the tail
	coordinate toDelete = snake[0];
	for (i = toDelete.y; i < toDelete.y + SNAKEHEIGHT; i++) {
		for (j = toDelete.x ; j < toDelete.x + SNAKEHEIGHT; j++) {
			frameBuffer[i * SIZEX + j] =  0x0000;
		}
	}
	
	// Move snake body square by square
	for (i = 0; i < snakeLength - 1; i++) {
		snake[i]= snake[i + 1];
	}
	
	// Update the head
	switch (snakeDirection) {
		case Up: snake[snakeLength - 1].y -= SNAKEHEIGHT;  break;
		case Right: snake[snakeLength - 1].x += SNAKEHEIGHT; break;
		case Left: snake[snakeLength - 1].x -= SNAKEHEIGHT; break;
		case Down: snake[snakeLength - 1].y += SNAKEHEIGHT; break;
	}
	
	// Print snake
	for (k = 0; k < snakeLength; k++) {
		for (i = snake[k].y; i < snake[k].y + SNAKEHEIGHT; i++) {
			for (j = snake[k].x ; j < snake[k].x + SNAKEHEIGHT; j++) {
				frameBuffer[i * SIZEX + j] = BLUE;
			}
		}		
	}	

	checkFood();
	checkSnake();
	
	if(!gameOverVar) ioctl(lcdDriver, 0x4680, &shape);
}	

//====== Check if a border is hit ======//
void checkBorder()
{
	int i;
	int j;
	
	for (i= 0; i < SNAKEHEIGHT; i++)
	{
		for (j= 0; j < SNAKEHEIGHT; j++)
		{	
			if (  !(snake[snakeLength - 1].x + i > BORDERTHICKNESS && snake[snakeLength - 1].x + i< SIZEX - BORDERTHICKNESS)
	   		   || !(snake[snakeLength - 1].y + j > BORDERTHICKNESS && snake[snakeLength - 1].y + j < SIZEY - BORDERTHICKNESS)  ) gameOver();
	   	}  	   
	 }
}	   

//====== Check if food is eaten and create a new one ======//
void checkFood()
{
	int i;
	int j;
	for (i= 0; i < SNAKEHEIGHT; i++)
	{
		for (j= 0; j < SNAKEHEIGHT; j++)
		{	
			if (  (snake[snakeLength - 1].x + i > food.x && snake[snakeLength - 1].x + i< food.x + SNAKEHEIGHT)
	   		   && (snake[snakeLength - 1].y + j > food.y && snake[snakeLength - 1].y + j < food.y + SNAKEHEIGHT)  ){
	   
	    		// Remove food from the buffer
				for (i = food.y; i < food.y + SNAKEHEIGHT; i++) 
				{
					for (j = food.x ; j < food.x + SNAKEHEIGHT; j++) 
					{
						frameBuffer[i * SIZEX + j] = 0x0000;
					}
				}
				
				foodNumber +=1;	
				
				// Create new food
				randcoor.x = rand() % (319 - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
				randcoor.y = rand() % (239  - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
				
				// Create a normal or a special food
				if(foodNumber % 3 !=0)
				{	
					for (i = randcoor.y; i < randcoor.y + SNAKEHEIGHT ; i++) 
					{
						for (j = randcoor.x; j < randcoor.x + SNAKEHEIGHT; j++) 
						{
							frameBuffer[i * SIZEX + j] = YELLOW;
						}
					}
				}
				else
				{
					for (i = randcoor.y; i < randcoor.y + SNAKEHEIGHT ; i++) 
					{
						for (j = randcoor.x; j < randcoor.x + SNAKEHEIGHT; j++) 
						{
							frameBuffer[i * SIZEX + j] = GREEN;
						}
					}
				}	
				
				// If ate a normal food, grow by 1. If ate special food, grow by 3
				if((foodNumber-1) % 3 !=0)
				{	
					snakeLength += 1;
					// Add new block	
					if(snakeDirection == Left)
					{
	    					snake[snakeLength-1].x = snake[snakeLength-2].x - SNAKEHEIGHT;
	    					snake[snakeLength-1].y = snake[snakeLength-2].y;
					}
					else if(snakeDirection == Right)
					{
	    					snake[snakeLength-1].x = snake[snakeLength-2].x + SNAKEHEIGHT;
	    					snake[snakeLength-1].y = snake[snakeLength-2].y;
					}	
					else if(snakeDirection == Up)
					{
	    					snake[snakeLength-1].x = snake[snakeLength-2].x;
	    					snake[snakeLength-1].y = snake[snakeLength-2].y - SNAKEHEIGHT;
					}
					else
					{	
	    					snake[snakeLength-1].x = snake[snakeLength-2].x ;
	    					snake[snakeLength-1].y = snake[snakeLength-2].y + SNAKEHEIGHT;
					}	
				}
				else
				{
					snakeLength += 3;
					// Add 3 new blocks	
					if(snakeDirection == Left)
					{
	    					snake[snakeLength-3].x = snake[snakeLength-4].x - SNAKEHEIGHT;
	    					snake[snakeLength-3].y = snake[snakeLength-4].y;
	    					snake[snakeLength-2].x = snake[snakeLength-4].x - 2 * SNAKEHEIGHT;
	    					snake[snakeLength-2].y = snake[snakeLength-4].y;
	    					snake[snakeLength-1].x = snake[snakeLength-4].x - 3 * SNAKEHEIGHT;
	    					snake[snakeLength-1].y = snake[snakeLength-4].y;

					}
					else if(snakeDirection == Right)
					{
	    					snake[snakeLength-3].x = snake[snakeLength-4].x + SNAKEHEIGHT;
	    					snake[snakeLength-3].y = snake[snakeLength-4].y;
	    					snake[snakeLength-2].x = snake[snakeLength-4].x + 2 * SNAKEHEIGHT;
	    					snake[snakeLength-2].y = snake[snakeLength-4].y;
	    					snake[snakeLength-1].x = snake[snakeLength-4].x + 3 * SNAKEHEIGHT;
	    					snake[snakeLength-1].y = snake[snakeLength-4].y;
					}	
					else if(snakeDirection == Up)
					{
	    					snake[snakeLength-3].x = snake[snakeLength-4].x;
	    					snake[snakeLength-3].y = snake[snakeLength-4].y - SNAKEHEIGHT;
	    					snake[snakeLength-2].x = snake[snakeLength-4].x;
	    					snake[snakeLength-2].y = snake[snakeLength-4].y - 2*SNAKEHEIGHT;
	    					snake[snakeLength-1].x = snake[snakeLength-4].x;
	    					snake[snakeLength-1].y = snake[snakeLength-4].y - 3*SNAKEHEIGHT;
					}
					else
					{	
	    					snake[snakeLength-3].x = snake[snakeLength-4].x;
	    					snake[snakeLength-3].y = snake[snakeLength-4].y + SNAKEHEIGHT;
	    					snake[snakeLength-2].x = snake[snakeLength-4].x;
	    					snake[snakeLength-2].y = snake[snakeLength-4].y + 2*SNAKEHEIGHT;
	    					snake[snakeLength-1].x = snake[snakeLength-4].x;
	    					snake[snakeLength-1].y = snake[snakeLength-4].y + 3*SNAKEHEIGHT;
					}
				}		
				
				// Update the coordinates for the new food
				food.x = randcoor.x;
				food.y = randcoor.y;
																								
	    	}
		}
	}
}	

//====== Check if the snake bite itself ======//
void checkSnake()
{
	int i;
	int j;
	int k;
	
	for (i= 0; i < SNAKEHEIGHT; i++)
	{
		for (j= 0; j < SNAKEHEIGHT; j++)
		{
			for(k=0; k<snakeLength-2;k++)
			{
				if (  (snake[snakeLength - 1].x + i > snake[k].x && snake[snakeLength - 1].x + i< snake[k].x + SNAKEHEIGHT)
	   			   && (snake[snakeLength - 1].y + j > snake[k].y && snake[snakeLength - 1].y + j < snake[k].y + SNAKEHEIGHT)  )
				   {	
						gameOver();
					}
			}	

		}
	}
}

//====== Finish the game ======//
void gameOver()
{
	gameOverVar = true;
	deathScreen();
}
