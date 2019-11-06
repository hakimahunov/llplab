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

//====== Local variables ======//
#define SCREENSIZE		240 * 320 
#define BASECELLSIZE	12
#define FILESIZE		(SCREENSIZE) * sizeof(short) //Size in bytes of the frame buffer
#define RED				0xf800
#define GREEN			0x0400
#define BLUE			0x001f		
#define YELLOW			0xffe0
#define WHITE			0xffff
#define BTN_CHECKER(shift_amount, state) ((1<<(shift_amount)) & ~(state))
#define BORDERTHICKNESS 3
#define SNAKEHEIGHT 	6
#define CENTERX			160
#define CENTERY			120
#define foodWidth       3

int headx;
int heady;
int tailx;
int taily;
int lcdDriver;
short* frameBuffer;
struct fb_copyarea shape;
FILE* dev;
enum Direction {Left = 4, Up = 5, Right = 6, Down = 7};
int snakeDirection = Up; 
int snakeLength = 3;
struct coordinate
{
    int x;
    int y;
    //int direction;
};
typedef struct coordinate coordinate;
coordinate snake[50];
coordinate food;
bool gameOverVar = false;
 

//====== Local functions ======//
void initScreen();
void pushToScreen(short val, int x, int y);
void cleanScreen();
int setGpadDriver();
void inputHandler(int signal);
void initSnake();
void moveSnake();
void moveSnakeRight();
void moveSnakeUp();
void checkBorder();
void gameOver();
void checkFood();
int main(int argc, char *argv[])
{

    initScreen();
    cleanScreen();
    initSnake();

    int result = setGpadDriver();
    if (result == EXIT_FAILURE) {
		printf("Driver initialization FAILED.");
		exit(EXIT_FAILURE);
	}
		
	while (!gameOverVar) {
		//sleep(200);
		moveSnake();
	}
 	exit(EXIT_SUCCESS);
}

//====== Driver related functions section ======//
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
	
	//moveSnake();
}


//====== LCD screen functions section ======//
void initScreen()
{
	lcdDriver = open("/dev/fb0", 2);
	frameBuffer = (short*) mmap(NULL, FILESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, lcdDriver, 0);
	
	// Default rectangle size
	shape.width = BASECELLSIZE;
	shape.height = BASECELLSIZE;
}

void pushToScreen(short val, int x, int y)
{
	int a;
	int b;
	
	for (a = y * BASECELLSIZE; a < (y*BASECELLSIZE + BASECELLSIZE); a++) {
		for (b = x * BASECELLSIZE; b < (x*BASECELLSIZE + BASECELLSIZE); b++) {
			frameBuffer[a * 320 + b] = val;
		}
	}

	shape.dx = x * BASECELLSIZE;
	shape.dy = y * BASECELLSIZE;
	
	ioctl(lcdDriver, 0x4680, &shape);
}

void cleanScreen()
{
	shape.dx = 0;
	shape.dy = 0;
	shape.width = 320;
	shape.height = 240;
	
	int i;
	for (i = 0; i < SCREENSIZE; i++) {
		frameBuffer[i] = 0x0000;
	}
	
	ioctl(lcdDriver, 0x4680, &shape);
	
	shape.width = BASECELLSIZE;
	shape.height = BASECELLSIZE;
}

void initSnake()
{
	int a;
	int b;
	
	shape.dx = 0;
	shape.dy = 0;
	shape.width = 320;
	shape.height = 240;
	
	// Create the snake
	for (a = CENTERY; a < CENTERY + SNAKEHEIGHT; a++) {  
		for (b = CENTERX; b < CENTERX + 3 * SNAKEHEIGHT; b++) { 
			frameBuffer[a * 320 + b] = BLUE;
		}
	}
	for (a = 0; a < snakeLength; a++) {
		snake[a].x = CENTERX + a * SNAKEHEIGHT;
		snake[a].y = CENTERY;
	}
	
	// Create the border
	for (a = 0; a < BORDERTHICKNESS; a++) {
		for (b = 0; b < 320; b++) {
			frameBuffer[a * 320 + b] = RED;
		}
	}
	for (a = 0; a < 240; a++) {
		for (b = 0; b < BORDERTHICKNESS; b++) {
			frameBuffer[a * 320 + b] = RED;
		}
	}		
	for (a = 240 - BORDERTHICKNESS; a < 240; a++) {
		for (b = 0; b < 320; b++) {
			frameBuffer[a * 320 + b] = RED;
		}
	}
	for (a = 0; a < 240; a++) {
		for (b = 320 - BORDERTHICKNESS; b < 320; b++) {
			frameBuffer[a * 320 + b] = RED;
		}
	}
	
	// Creating initial food item
	srand(time(NULL));
	int randx = rand() % (320 - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
	int randy = rand() % (240  - BORDERTHICKNESS - SNAKEHEIGHT) + 3;
	for (a = randy; a < randy + SNAKEHEIGHT ; a++) {
		for (b = randx; b < randx + SNAKEHEIGHT; b++) {
			frameBuffer[a * 320 + b] = YELLOW;
		}
	}	
	
	food.x = randx;
	food.y = randy;
	ioctl(lcdDriver, 0x4680, &shape);
}	

//====== Snake functions section ======//

void moveSnake()
{

	
	int i;
	int j;
	int k;
	shape.dx = 0;
	shape.dy = 0;
	shape.width = 320;
	shape.height = 240;	
	
	checkBorder();
	checkFood();
	// Delete the tail
	coordinate toDelete = snake[0];
	for (i = toDelete.y; i < toDelete.y + SNAKEHEIGHT; i++) {
		for (j = toDelete.x ; j < toDelete.x + SNAKEHEIGHT; j++) {
			frameBuffer[i * 320 + j] =  0x0000;
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
				frameBuffer[i * 320 + j] = BLUE;
			}
		}		
	}	
	
	ioctl(lcdDriver, 0x4680, &shape);
}	

void checkBorder()
{
	
	if (  !(snake[snakeLength - 1].x > BORDERTHICKNESS && snake[snakeLength - 1].x <320 - BORDERTHICKNESS)
	   || !(snake[snakeLength - 1].y > BORDERTHICKNESS && snake[snakeLength - 1].y <240 - BORDERTHICKNESS)  ) gameOver();
	   
}	   

void gameOver(){
	gameOverVar = true;
	cleanScreen();
}

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
	   
	    			gameOver();
	    			
	    			}
	    		}
	   }
	   // Remove food from frame buffer
	   // Increase snake length
	   // Add a new snake block
	   // Produce new food and update food coordinates
	   // Update the screen	   
}	








