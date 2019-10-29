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


int lcdDriver;
short* frameBuffer;
struct fb_copyarea shape;
FILE* dev;
enum Direction {Left = 4, Up = 5, Right = 6, Down = 7};


//====== Local functions ======//
void initScreen();
void pushToScreen(short val, int x, int y);
void cleanScreen();
int setGpadDriver();
void inputHandler(int signal);


int main(int argc, char *argv[])
{
    initScreen();
    cleanScreen();
        
    int result = setGpadDriver();
    if (result == EXIT_FAILURE) {
		printf("Driver initialization FAILED.");
		exit(EXIT_FAILURE);
	}
		
	while (1) {
		__asm__("WFI");
	}
 	exit(EXIT_SUCCESS);
}


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
		pushToScreen(RED, 5, 5);
	}
	else if (BTN_CHECKER(Up, input)) {
		pushToScreen(GREEN, 6, 6);
	}
	else if (BTN_CHECKER(Right, input)) {
		pushToScreen(YELLOW, 7, 7);
	}
	else if (BTN_CHECKER(Down, input)) {
		pushToScreen(BLUE, 8, 8);
	}
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
